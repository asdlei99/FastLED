#include "third_party/arduinojson/json.h"
#include "platforms/wasm/js.h"
#include "ui_manager.h"

FASTLED_NAMESPACE_BEGIN

jsButton::jsButton(const char* name)
    : mPressed(false) {
    auto updateFunc = [this](const char* jsonStr) { this->updateInternal(jsonStr); };
    auto toJsonFunc = [this](ArduinoJson::JsonObject& json) { this->toJson(json); };
    mInternal = std::make_shared<jsUiInternal>(name, std::move(updateFunc), std::move(toJsonFunc));
    jsUiManager::addComponent(mInternal);
    mUpdater.init(this);
}

jsButton::~jsButton() {
    jsUiManager::removeComponent(mInternal);
}

const char* jsButton::name() const {
    return mInternal->name();
}

void jsButton::toJson(ArduinoJson::JsonObject& json) const {
    json["name"] = name();
    json["type"] = "button";
    json["id"] = mInternal->id();
    json["pressed"] = mPressed;
}

bool jsButton::isPressed() const {
    // Due to ordering of operations, mPressedLast is always equal to
    // mPressed. So we kind of fudge a little on the isPressed() event
    // here;
    return mPressed || mClickedHappened;
}

void jsButton::updateInternal(const char* jsonStr) {
    ArduinoJson::JsonDocument doc;
    ArduinoJson::DeserializationError error = ArduinoJson::deserializeJson(doc, jsonStr);
    if (!error) {
        mPressed = doc["pressed"].as<bool>();
    }
}

FASTLED_NAMESPACE_END
