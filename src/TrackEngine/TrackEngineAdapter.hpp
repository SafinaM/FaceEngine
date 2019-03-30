#pragma once
#include <string>
#include "../FaceEngineAdapter.hpp"
#include "trackEngine/ITrackEngine.h"
#include "trackEngine/ITrackCallbacks.h"
#include "trackEngine/TrackEngineTypes.h"
#include "TrackEngineCallback.hpp"
#include "CallbackObserver.hpp"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <mutex>

class PyIStream;
class PyITrackEngine {
public:
	fsdk::Ref<tsdk::ITrackEngine> trackEngine;

	PyITrackEngine(const PyIFaceEngine& fsdk, const std::string& configPath);
	std::string getStr();
	std::string getFSDKVer(const PyIFaceEngine& fsdk);
	PyIStream createStream();
};

class PyIStream {
public:
	PyIStream(PyIStream& other):
			stream{other.stream},
			streamObserver{other.streamObserver}
	{}

	PyIStream(PyIStream&& other) noexcept:
			stream{other.stream},
			streamObserver{std::move(other.streamObserver)}
	{}

	explicit PyIStream(fsdk::Ref<tsdk::IStream>&& _stream);

	void pushFrame(const fsdk::Image& image);
	std::vector<PyICallback> getCallbacks();
private:
	fsdk::Ref<tsdk::IStream> stream;
	std::shared_ptr<Observer> streamObserver;
};

