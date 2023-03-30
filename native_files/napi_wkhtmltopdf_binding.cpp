#include <napi.h>
#include <wkhtmltox/pdf.h>
#include <iostream>

class PdfGenerator : public Napi::ObjectWrap<PdfGenerator> {

public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    PdfGenerator(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;

    void Generate(const Napi::CallbackInfo& info);
    void SetHTML(const Napi::CallbackInfo& info);
    void SetURL(const Napi::CallbackInfo& info);
    void SetGlobalSettings(const Napi::CallbackInfo& info);
    void SetObjectSettings(const Napi::CallbackInfo& info);
    void SetOutputAsBuffer(const Napi::CallbackInfo& info);
    void SetOutput(const Napi::CallbackInfo& info);

    wkhtmltopdf_converter* converter_;
    wkhtmltopdf_global_settings* gs;
    wkhtmltopdf_object_settings* os;
    std::vector<uint8_t> pdf_data_;    
};

Napi::FunctionReference PdfGenerator::constructor;

Napi::Object PdfGenerator::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "PdfGenerator", {
        InstanceMethod("generate", &PdfGenerator::Generate),
        InstanceMethod("setHTML", &PdfGenerator::SetHTML),
        InstanceMethod("setURL", &PdfGenerator::SetURL),
        InstanceMethod("setGlobalSettings", &PdfGenerator::SetGlobalSettings),
        InstanceMethod("setObjectSettings", &PdfGenerator::SetObjectSettings),
        InstanceMethod("setOutputAsBuffer", &PdfGenerator::SetOutputAsBuffer),
        InstanceMethod("setOutput", &PdfGenerator::SetOutput),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("PdfGenerator", func);

    return exports;
}

PdfGenerator::PdfGenerator(const Napi::CallbackInfo& info) : Napi::ObjectWrap<PdfGenerator>(info) {
    wkhtmltopdf_init(0);
    gs = wkhtmltopdf_create_global_settings();
    os = wkhtmltopdf_create_object_settings();
    converter_ = wkhtmltopdf_create_converter(gs);
}

void PdfGenerator::SetHTML(const Napi::CallbackInfo& info) {
    if (!info[0].IsString()) {
        Napi::TypeError::New(info.Env(), "HTML not found!")
            .ThrowAsJavaScriptException();
        return;
    }
    std::string html = "data:text/html,<!DOCTYPE html>";
    html.append(info[0].ToString().Utf8Value());
    wkhtmltopdf_set_object_setting(os, "page", html.c_str());
}

void PdfGenerator::SetURL(const Napi::CallbackInfo& info) {
    if (!info[0].IsString()) {
        Napi::TypeError::New(info.Env(), "URL not found!").ThrowAsJavaScriptException();
        return;
    }
    std::string value = "";
    value.append(info[0].ToString().Utf8Value());
    wkhtmltopdf_set_object_setting(os, "page", value.c_str());
}

void PdfGenerator::SetGlobalSettings(const Napi::CallbackInfo& info) {
    if (!info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(info.Env(), "Both arguments must be string!")
            .ThrowAsJavaScriptException();
        return;
    }
    std::string key = "";
    key.append(info[0].ToString().Utf8Value());

    std::string value = "";
    value.append(info[1].ToString().Utf8Value());

    wkhtmltopdf_set_global_setting(gs, key.c_str(), value.c_str());
}

void PdfGenerator::SetObjectSettings(const Napi::CallbackInfo& info) {
    if (!info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(info.Env(), "Both arguments must be string!")
            .ThrowAsJavaScriptException();
        return;
    }
    std::string key = "";
    key.append(info[0].ToString().Utf8Value());

    std::string value = "";
    value.append(info[1].ToString().Utf8Value());

    wkhtmltopdf_set_object_setting(os, key.c_str(), value.c_str());
}

void PdfGenerator::SetOutputAsBuffer(const Napi::CallbackInfo& info) {
    wkhtmltopdf_set_global_setting(gs, "out", NULL);
}

void PdfGenerator::SetOutput(const Napi::CallbackInfo& info) {
    if (!info[0].IsString()) {
        Napi::TypeError::New(info.Env(), "Ouput path is required!")
            .ThrowAsJavaScriptException();
        return;
    }
    std::string value = "";
    value.append(info[0].ToString().Utf8Value());
    wkhtmltopdf_set_global_setting(gs, "out", value.c_str());
}

void PdfGenerator::Generate(const Napi::CallbackInfo& info) {
    wkhtmltopdf_add_object(converter_, os, NULL);

    if (!wkhtmltopdf_convert(converter_))
        if (info.Length() > 1 && info[1].IsFunction()) {
            Napi::Function callback = info[1].As<Napi::Function>();
            callback.Call(info.This(), {});
        }

    const uint8_t* data;
    if (info.Length() > 0 && info[0].IsFunction()) {
        size_t len = wkhtmltopdf_get_output(converter_, &data);
        pdf_data_.resize(len);
        memcpy(pdf_data_.data(), data, len);
    }

    wkhtmltopdf_destroy_converter(converter_);
    wkhtmltopdf_destroy_global_settings(gs);
    wkhtmltopdf_destroy_object_settings(os);

    if (info.Length() > 0 && info[0].IsFunction()) {
        Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::New(info.Env(), pdf_data_.data(), pdf_data_.size());
        Napi::Function callback = info[0].As<Napi::Function>();
        callback.Call(info.This(), { buffer });
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    PdfGenerator::Init(env, exports);
    return exports;
}

NODE_API_MODULE(addon, Init)
