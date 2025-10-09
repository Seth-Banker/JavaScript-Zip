#include <napi.h>
#include <vector>
#include <string>
#include "../zipper.h"
#include "../unzipper.h"


namespace JavaScript_Zip {
    // Convert JS string[] -> std::vector<std::string>
    static std::vector<std::string> toPaths(const Napi::Array& arr) {
        std::vector<std::string> out;
        out.reserve(arr.Length());
        for (uint32_t i = 0; i < arr.Length(); ++i) {
            auto v = arr.Get(i);
            if (!v.IsString()) throw Napi::TypeError::New(arr.Env(), "paths must be strings");
            out.push_back(v.As<Napi::String>().Utf8Value());
        }
        return out;
    }

    // Buffer = zipFromFiles(string[] paths)
    static Napi::Value ZipFromFiles(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        if (info.Length() != 1 || !info[0].IsArray())
            throw Napi::TypeError::New(env, "Usage: zipFromFiles(string[])");

        auto paths = toPaths(info[0].As<Napi::Array>());
        try {
            auto bytes = zip_from_files(paths);
            return Napi::Buffer<uint8_t>::Copy(env, bytes.data(), bytes.size());
        }
        catch (const std::exception& e) {
            throw Napi::Error::New(env, e.what());
        }
    }

    // void = zipFilesToPath(string[] paths, string outZipPath)
    static Napi::Value ZipFilesToPath(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        if (info.Length() != 2 || !info[0].IsArray() || !info[1].IsString())
            throw Napi::TypeError::New(env, "Usage: zipFilesToPath(string[], string)");

        auto paths = toPaths(info[0].As<Napi::Array>());
        auto outPath = info[1].As<Napi::String>().Utf8Value();
        try {
            JavaScript_Zip::zip_files_to_path(paths, outPath);
            return env.Undefined();
        }
        catch (const std::exception& e) {
            throw Napi::Error::New(env, e.what());
        }
    }

    // unzip(buffer) -> { [path]: Buffer }
    Napi::Value UnzipJS(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        if (info.Length() < 1 || !info[0].IsBuffer())
            Napi::TypeError::New(env, "expected Buffer").ThrowAsJavaScriptException();

        if (env.IsExceptionPending()) return env.Undefined();

        auto buf = info[0].As<Napi::Buffer<uint8_t>>();
        try {
            auto files = unzipAllFromMemory(buf.Data(), buf.Length());
            Napi::Object result = Napi::Object::New(env);
            for (const auto& f : files) {
                Napi::Buffer<uint8_t> b = Napi::Buffer<uint8_t>::Copy(env, f.data.data(), f.data.size());
                result.Set(Napi::String::New(env, f.name), b);
            }
            return result;
        }
        catch (const std::exception& e) {
            Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
            return env.Undefined();
        }
    }

    // unzipToDisk(buffer, outDir) -> void
    Napi::Value UnzipToDiskJS(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
        if (info.Length() < 2 || !info[0].IsBuffer() || !info[1].IsString())
            Napi::TypeError::New(env, "expected (Buffer, string)").ThrowAsJavaScriptException();

        if (env.IsExceptionPending()) return env.Undefined();

        auto buf = info[0].As<Napi::Buffer<uint8_t>>();
        auto outDir = info[1].As<Napi::String>().Utf8Value();

        try {
            auto files = unzipAllFromMemory(buf.Data(), buf.Length());
            writeAllToDisk(files, outDir);
            return env.Undefined();
        }
        catch (const std::exception& e) {
            Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
            return env.Undefined();
        }
    }



    Napi::Object Init(Napi::Env env, Napi::Object exports) {
        exports.Set("zip", Napi::Function::New(env, ZipFromFiles));
        exports.Set("zipDisk", Napi::Function::New(env, ZipFilesToPath));
        exports.Set("unzip", Napi::Function::New(env, UnzipJS));
        exports.Set("unzipDisk", Napi::Function::New(env, UnzipToDiskJS));
        return exports;
    }

    // The name here is arbitrary; it doesn't have to match the filename.
    NODE_API_MODULE(zip_addon, Init);
}