#include <iostream>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"
#include "LivenessEngineAdapter.hpp"
#include "SettingsProviderAdapter.hpp"
#include "helpers.hpp"
#include <fsdk/Version.h>

namespace py = pybind11;

void estimators_module(py::module& f);
void index_module(py::module& f);
void image_rect_module(py::module &f);
void settings_provider_module(py::module& f);
void detector_module(py::module& f);
void descriptor_module(py::module& f);
void warper_module(py::module& f);
void liveness_module(py::module& f);

PyIFaceEngine createPyFaceEnginePtr(const char* dataPath = nullptr, const char* configPath = nullptr) {
	return PyIFaceEngine(dataPath, configPath);
}

PyILivenessEngine createPyLivenessEnginePtr(
	const PyIFaceEngine& pyIFaceEngine,
	const char* dataPath = nullptr) {
	return PyILivenessEngine(pyIFaceEngine, dataPath);
}

PyISettingsProvider createSettingsProviderPtr(const char* path) {
	return PyISettingsProvider(path);
}

template<class T>
py::class_<fsdk::Optional<T>> optional_class(py::module& this_module, const char* name)
{
	py::class_<fsdk::Optional<T>>class_instance(this_module, name);
	
	class_instance.def("value", [](const fsdk::Optional<T>& self) {
			return self.value();
		});
	class_instance.def("isValid", [](const fsdk::Optional<T>& self) {
		return self.valid();
	});
	
	return class_instance;
}

void set_optional_class(py::module& f)
{
	auto optionalLandmarks5 = optional_class<fsdk::Landmarks5>(f, "OptionalLandmarks5");
	auto optionalLandmarks68 = optional_class<fsdk::Landmarks68>(f, "OptionalLandmarks68");
	auto optionalfloat = optional_class<float>(f, "Optionalfloat");
	auto optionalEthnicityEstimation = optional_class<fsdk::EthnicityEstimation>(f, "OptionalEthnicityEstimation");

}


PYBIND11_MODULE(FaceEngine, f) {
	
	estimators_module(f);
	index_module(f);
	image_rect_module(f);
	settings_provider_module(f);
	detector_module(f);
	descriptor_module(f);
	warper_module(f);
	liveness_module(f);
	set_optional_class(f);
	
	enum class FaceEngineEdition {
		FrontEndEdition,
		CompleteEdition
	};

	py::enum_<fsdk::RecognitionMode>(f, "RecognitionMode", py::arithmetic(), "Recognition mode type.\n")
		.value("RM_RGB", fsdk::RM_RGB, "RGB image processing mode (default mode)")
		.value("RM_INFRA_RED", fsdk::RM_INFRA_RED, "IR image processing mode")
		.export_values();
			;

	py::enum_<fsdk::ObjectDetectorClassType>(f, "ObjectDetectorClassType", py::arithmetic(), "Object detector type enumeration.\n")
		.value("FACE_DET_DEFAULT", fsdk::FACE_DET_DEFAULT, "Default detector cpecified in config file")
		.value("FACE_DET_V1", fsdk::FACE_DET_V1, "First detector type")
		.value("FACE_DET_V2", fsdk::FACE_DET_V2, "Light detector type")
		.value("FACE_DET_V3", fsdk::FACE_DET_V3, "Third detector type")
		.export_values();
			;

	py::class_<fsdk::Face>(f, "Face", "Container for detection and landmakrs\n")
		.def(py::init<>())
		.def(py::init<fsdk::Image>())
		.def_readwrite("img", &fsdk::Face::m_img, "Image\n")
		.def_readwrite("detection", &fsdk::Face::m_detection, "Detection\n")
		.def_readwrite("landmarks5_opt", &fsdk::Face::m_landmarks5, "Landmarks5 optinal\n")
		.def_readwrite("landmarks68_opt", &fsdk::Face::m_landmarks68, "Landmarks68 optinal\n")
		.def("isValid", &fsdk::Face::isValid, "Valid or not\n")
			;
	
	py::enum_<fsdk::FaceEngineEdition>(f, "FaceEngineEdition", "Complete or FrontEdition version.\n")
		.value("FrontEndEdition", fsdk::FaceEngineEdition::FrontEndEdition)
		.value("CompleteEdition", fsdk::FaceEngineEdition::CompleteEdition)
		.export_values();
			;
	
	f.def("getVersionHash", []() -> std::string {
		return std::string("fsdk_hash: ") + fsdk::getVersionHash();
	});
	f.def("getVersionString", []() -> std::string {
		return std::string("fsdk_version: ") + fsdk::getVersionString();
	});
	f.def("getBuildInfo", []() -> std::string {
		return std::string("fsdk_build_info: ") + fsdk::getBuildInfo();
	});
	
	f.def("createFaceEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		"Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr,
		"Create the LUNA SDK root object\n"
		"\tArgs:\n"
		"\t\tparam1 (str): [optional] path to folder with FSDK data.\n"
		"\t\tparam2 (str): [optional] path to faceengine.conf file. Default: <dataPath>/faceengine.cong\n");
	
	f.def("createSettingsProvider", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		"Create object SettingsProvider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): configuration file path\n");
	
	f.def("createLivenessEngine", &createPyLivenessEnginePtr, py::return_value_policy::take_ownership,
		  "Create the Liveness object\n"
		"\tArgs:\n"
		"\t\tparam1 (FaceEngine obj): the LUNA SDK root object.\n"
		"\t\tparam2 (str): [optional] path to folder with FSDK data.\n");
	
	py::class_<PyIFaceEngine>(f, "PyIFaceEngine", "Root LUNA SDK object interface\n")
		.def("getFaceEngineEdition", &PyIFaceEngine::getFaceEngineEdition,
			"Get LUNA SDK ditribution edition.\n"
			"\tReturns:\n"
			"\t\t(enum FaceEngineEdition) Edition of LUNA SDK instance. Posible values:\n"
			"\t\t\tCompleteEdition - full edition with all function.\n"
			"\t\t\tFrontEndEdition - short edition with excluded descriptor functions.\n"
			"\t\t\tExtractor, matcher, descriptor, batch and index cannot be created.")
		.def("createAttributeEstimator", &PyIFaceEngine::createAttributeEstimator, "Creates Attribute estimator\n")
		.def("createQualityEstimator", &PyIFaceEngine::createQualityEstimator, "Creates Quality estimator\n")
		.def("createEthnicityEstimator", &PyIFaceEngine::createEthnicityEstimator, "Creates Ethnicity estimator\n")
		
		.def("createHeadPoseEstimator", &PyIFaceEngine::createHeadPoseEstimator, "Creates Head pose estimator\n")
		.def("createBlackWhiteEstimator", &PyIFaceEngine::createBlackWhiteEstimator, "Creates Black/White estimator\n")
		.def("createDepthEstimator", &PyIFaceEngine::createDepthEstimator, "Creates Liveness Depth estimator\n")
		.def("createIREstimator", &PyIFaceEngine::createIREstimator, "Creates Liveness Infrared estimator\n")
		.def("createSmileEstimator", &PyIFaceEngine::createSmileEstimator, "Creates Smile estimator\n")
		.def("createFaceFlowEstimator", &PyIFaceEngine::createFaceFlowEstimator, "Creates Liveness flow estimator. \n"
			"Note: this estimator is required only for liveness detection purposes.\n")

		.def("createEyeEstimator", &PyIFaceEngine::createEyeEstimator, 
			"Creates Eye estimator of given recognition mode\n",
			py::arg("mode") = fsdk::RecognitionMode::RM_RGB,
			"\tArgs:\n"
			"\t\tparam2 (enum RecognitionMode): Recognition mode type enumeration.\n"
			"\tReturns:\n"
			"\t\tEye estimator instance.")

		.def("createEmotionsEstimator", &PyIFaceEngine::createEmotionsEstimator, "Creates Emotions estimator\n")
		.def("createGazeEstimator", &PyIFaceEngine::createGazeEstimator, "Creates Gaze estimator\n")
		.def("createGlassesEstimator", &PyIFaceEngine::createGlassesEstimator, "Creates glasses estimator\n")
		.def("createAGSEstimator", &PyIFaceEngine::createAGSEstimator, "Creates AGS estimator\n")

		.def("createDetector", &PyIFaceEngine::createDetector,
			"Creates a detector of given type.\n", 
			py::arg("type") = fsdk::ObjectDetectorClassType::FACE_DET_DEFAULT,
			py::arg("mode") = fsdk::RecognitionMode::RM_RGB,
			"\tArgs:\n"
			"\t\tparam1 (enum ObjectDetectorClassType): Object detector type enumeration.\n"
			"\t\tparam2 (enum RecognitionMode): Recognition mode type enumeration.\n"
			"\tReturns:\n"
			"\t\tDetector instance.")
		
		.def("createHumanDetector", &PyIFaceEngine::createHumanDetector,
			"Creates a human detector.\n")
		
		.def("createWarper", &PyIFaceEngine::createWarper, "Creates warper\n")
		.def("createDescriptor", &PyIFaceEngine::createDescriptor, "Creates Descriptor\n")
		.def("createDescriptorBatch", &PyIFaceEngine::createDescriptorBatch, py::arg("size"), py::arg("version") = 0,
			"Creates Batch of descriptors\n"
			"\tArgs:\n"
			"\t\tparam1 (int): amount of descriptors in batch\n"
			"\t\tparam2 (str): descriptor version in batch. If 0 - use default version from config\n")
		
		.def("createExtractor", &PyIFaceEngine::createExtractor, "Creates descriptor extractor\n")
		.def("createMatcher", &PyIFaceEngine::createMatcher, "Creates descriptor matcher\n")
		
		// Index
		.def("createIndexBuilder", &PyIFaceEngine::createIndexBuilder, "Creates index builder.\n")
		
		.def("loadDenseIndex", [](
			PyIFaceEngine& faceEngine,
			const char* indexPath) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndex*> res = faceEngine.loadDenseIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::acquire(res.getValue()));
				else
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::IDenseIndexPtr());
			},
			"Loads dense index.\n"
			"\t\t Only indexes saved as dense are to be loaded as dense.\n"
			"\tArgs:\n"
			"\t\t param1 (str): indexPath Path to index to be loaded.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and dense index): error code FSDKErrorResult and output dense index.\n")
		
		.def("loadDynamicIndex", [](
			PyIFaceEngine& faceEngine,
			const char* indexPath) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndex*> res = faceEngine.loadDynamicIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::acquire(res.getValue()));
				else
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::IDynamicIndexPtr());
			},
			"Loads dynamic index.\n"
			"\t\t Only indexes saved as dynamic are to be loaded as dynamic.\n"
			"\tArgs:\n"
			"\t\t param1 (str): indexPath Path to index to be loaded.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and dense index): error code FSDKErrorResult and output dynamic index.\n")
		
		.def("setSettingsProvider", &PyIFaceEngine::setSettingsProvider,
			"Sets settings provider\n"
			"\tArgs:\n"
			"\t\tparam1 (PyISettingsProvider): setting provider\n")

		.def("getLicense", &PyIFaceEngine::getLicense,
			"Get current License object, which was set for FaceEngine object.\n"
			"\tReturns:\n"
			"\t\t(fsdk::ILicensePtr): current license object."
			)

		.def("activateLicense", [](
			PyIFaceEngine& faceEngine,
			const fsdk::ILicensePtr& license,
			const char * licenseConfPath) {
				return faceEngine.activateLicense(license, licenseConfPath);
			},
			"Makes License activation with some platform specific manner. Network connection is required.\n"
			"\t\tThis method should be usend only on mobile or ARM platforms.\n"
			"\tArgs:\n"
			"\t\t param1 (fsdk::ILicensePtr): license object to activate.\n"
			"\t\t param2 (str): path to the license.conf file.\n"
			"\tReturns:\n"
			"\t\t (bool): True if license was successfully activated, False otherwise."
			)
				; // FaceEngine
	
	
	py::class_<fsdk::IFaceEnginePtr>(f, "IFaceEnginePtr");
	
	py::enum_<fsdk::ISerializableObject::Error>(f, "SerializeError", "Serialization error codes.\n")
		.value("Ok", fsdk::ISerializableObject::Error::Ok)
		.value("Size", fsdk::ISerializableObject::Error::Size)
		.value("Signature", fsdk::ISerializableObject::Error::Signature)
		.value("ArchiveRead", fsdk::ISerializableObject::Error::ArchiveRead)
		.value("InputArchive", fsdk::ISerializableObject::Error::InputArchive)
		.value("ArchiveWrite", fsdk::ISerializableObject::Error::ArchiveWrite)
			; // error

	//	Errors
	py::class_<SerializeErrorResult>(f, "SerializeErrorResult",
		"Wrapper for ISerializableObject::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &SerializeErrorResult::isOk)
		.def_readonly("isError", &SerializeErrorResult::isError)
		.def_readonly("serializeError", &SerializeErrorResult::serializeError)
		.def_readonly("what", &SerializeErrorResult::what)
		.def("__repr__",
			[](const SerializeErrorResult &err) {
				return "SerializeErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", serializeError = " +
						fsdk::ErrorTraits<fsdk::ISerializableObject::Error>::toString(err.serializeError)
						+ ", what = " + err.what; })
			;
	
	py::enum_<fsdk::IDescriptorBatch::Error>(f, "DescriptorBatchError",
		"Descriptor batch error enumeration.\n"
		"\tUsed for descriptor batch related errors indication.\n")
		.value("Ok", fsdk::IDescriptorBatch::Error::Ok)
		.value("InvalidInput", fsdk::IDescriptorBatch::Error::InvalidInput)
		.value("BatchFull", fsdk::IDescriptorBatch::Error::BatchFull)
		.value("Incompatible", fsdk::IDescriptorBatch::Error::Incompatible)
		.value("Internal", fsdk::IDescriptorBatch::Error::Internal)
		.value("IoError", fsdk::IDescriptorBatch::Error::IoError)
		.value("OutOfRange", fsdk::IDescriptorBatch::Error::OutOfRange)
			;
	
	py::class_<fsdk::Landmarks5>(f, "Landmarks5",
		"Face landmarks, length is fixed and equal to5.\n"
		"\tFace detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python built-in "
		"functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and research "
		"purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks5 &v) { return v.landmarkCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		.def("__getitem__", [](const fsdk::Landmarks5 &s, size_t i) {
				if (i >= s.landmarkCount) throw py::index_error();
				return s.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample: lanmarks[3]")
		
		.def("__setitem__", [](fsdk::Landmarks5 &s, size_t i, fsdk::Vector2<float> v) {
				if (i >= s.landmarkCount) throw py::index_error();
				s.landmarks[i].x = v.x;
				s.landmarks[i].y = v.y;
				return s.landmarks[i];
			},
			"Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
			; //Landmarks5
	
	py::class_<fsdk::Landmarks68>(f, "Landmarks68",
		"Face landmarks, length is fixed and equal to 68.\n"
		"\tFace detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python "
		"built-in functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and "
		"\tresearch purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks68 &v) { return v.landmarkCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		
		.def("__getitem__", [](const fsdk::Landmarks68 &s, size_t i) {
				if (i >= s.landmarkCount) throw py::index_error();
				return s.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample:lanmarks[3]")
		
		.def("__setitem__", [](fsdk::Landmarks68 &s, size_t i, fsdk::Vector2<float> v) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		}, "Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
		; //Landmarks68

// doc Template
//		"Te.\n"
//		"\tTe.\n"
//		"\tArgs\n"
//		"\t\tparam1 (t): te\n"
//		"\t\tparam2 (t): te\n"
//		"\tReturns:\n"
//		"\t\t(t): te"
// Vector2
	
	py::class_<fsdk::Vector2<float>>(f, "Vector2f", "Vector with 2 float values, x and y")
		.def(py::init<>())
		.def(py::init<float, float>())
		.def(py::init<const fsdk::Vector2<float>&>())
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def_readwrite("x", &fsdk::Vector2<float>::x)
		.def_readwrite("y", &fsdk::Vector2<float>::y)
		.def("__repr__",
			[](const fsdk::Vector2<float> &v) {
				return "x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y);
		})
		;
	
	py::class_<fsdk::Vector2<int>>(f, "Vector2i", "Vector with 2 integer values, x and y")
		.def(py::init<>())
		.def(py::init<int, int>())
		.def(py::init<const fsdk::Vector2<int>&>())
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def_readwrite("x", &fsdk::Vector2<int>::x)
		.def_readwrite("y", &fsdk::Vector2<int>::y)
		.def("__repr__",
			[](const fsdk::Vector2<int> &v) {
				return "x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y);
			})
		;

//	Errors
	py::class_<FSDKErrorResult>(f, "FSDKErrorResult",
		"Wrapper for FSDK::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &FSDKErrorResult::isOk)
		.def_readonly("isError", &FSDKErrorResult::isError)
		.def_readonly("FSDKError", &FSDKErrorResult::fsdkError)
		.def_readonly("what", &FSDKErrorResult::what)
		.def("__repr__",
			[](const FSDKErrorResult &err) {
				return "FSDKErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.fsdkError))
						+ ", what = " + err.what; })
			;
	
	//	Errors
	py::class_<LSDKErrorResult>(f, "LSDKErrorResult",
			"Wrapper for LSDK::Error that encapsulates an action result enumeration.\n"
			"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &LSDKErrorResult::isOk)
		.def_readonly("isError", &LSDKErrorResult::isError)
		.def_readonly("LSDKError", &LSDKErrorResult::lsdkError)
		.def_readonly("what", &LSDKErrorResult::what)
		.def("__repr__",
			 [](const LSDKErrorResult &err) {
				 return "LSDKErrorResult: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", LSDKError = " + std::to_string(static_cast<uint32_t>(err.lsdkError))
						+ ", what = " + err.what; })
		;
	
	py::class_<DescriptorBatchResult>(f, "DescriptorBatchResult",
		"Wrapper for DescriptorBatch::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &DescriptorBatchResult::isOk)
		.def_readonly("isError", &DescriptorBatchResult::isError)
		.def_readonly("DescriptorBatchError", &DescriptorBatchResult::descriptorBatchError)
		.def_readonly("what", &DescriptorBatchResult::what)
		.def("__repr__",
			[](const DescriptorBatchResult &err) {
				return "DescriptorBatchResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", DescriptorBatchError = " + std::to_string(static_cast<uint32_t>(err.descriptorBatchError))
						+ ", what = " + err.what; })
			;
	
	py::class_<ImageErrorResult>(f, "ImageErrorResult",
		"Wrapper for Image::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &ImageErrorResult::isOk)
		.def_readonly("isError", &ImageErrorResult::isError)
		.def_readonly("imageError", &ImageErrorResult::imageError)
		.def_readonly("what", &ImageErrorResult::what)
		.def("__repr__",
			[](const ImageErrorResult &err) {
				return "ImageErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", imageError = " + std::to_string(static_cast<uint32_t>(err.imageError))
						+ ", what = " + err.what; })
			;
	
	py::class_<SettingsProviderResult>(f, "SettingsProviderErrorResult",
		"Wrapper for SettingsProvider::Error that encapsulates an action result enumeration.\n"
		"\ttAn enum should specify a result code.\n")
		.def_readonly("isOk", &SettingsProviderResult::isOk)
		.def_readonly("isError", &SettingsProviderResult::isError)
		.def_readonly("SettingsProviderResult", &SettingsProviderResult::settingsProviderError)
		.def_readonly("what", &SettingsProviderResult::what)
		.def("__repr__",
			[](const SettingsProviderResult &err) {
				return "SettingsProviderResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", SettingsProviderError = " + std::to_string(static_cast<uint32_t>(err.settingsProviderError))
						+ ", what = " + err.what; })
		;
	
	py::class_<FSDKErrorValueInt>(f, "FSDKErrorValueInt", "Wrapper for result to output some integer "
		"value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueInt::isOk)
		.def_readonly("isError", &FSDKErrorValueInt::isError)
		.def_readonly("FSDKError", &FSDKErrorValueInt::fsdkError)
		.def_readonly("what", &FSDKErrorValueInt::what)
		.def_readonly("value", &FSDKErrorValueInt::value)
		.def("__repr__",
			[](const FSDKErrorValueInt &err) {
				return "FSDKErrorValueInt: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.fsdkError))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what; })
			;
	
	py::class_<FSDKErrorValueBool>(f, "FSDKErrorValueBool", "Wrapper for result to output some bool "
		"value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueBool::isOk)
		.def_readonly("isError", &FSDKErrorValueBool::isError)
		.def_readonly("FSDKError", &FSDKErrorValueBool::fsdkError)
		.def_readonly("what", &FSDKErrorValueBool::what)
		.def_readonly("value", &FSDKErrorValueBool::value)
		.def("__repr__",
			 [](const FSDKErrorValueBool &err) {
				 return "FSDKErrorValueBool: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.fsdkError))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what; })
			;
	
	py::class_<FSDKErrorValueFloat>(f, "FSDKErrorValueFloat",
		"Wrapper for result to output some "
		"float value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueFloat::isOk)
		.def_readonly("isError", &FSDKErrorValueFloat::isError)
		.def_readonly("FSDKError", &FSDKErrorValueFloat::fsdkError)
		.def_readonly("what", &FSDKErrorValueFloat::what)
		.def_readonly("value", &FSDKErrorValueFloat::value)
		.def("__repr__",
			[](const FSDKErrorValueFloat &err) {
				return "FSDKErrorValueFloat: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.fsdkError))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what;
			})
		;
	
	py::class_<FSDKErrorValueMatching>(f, "FSDKErrorValueMatching", "Wrapper for result to output "
		"some Matching value (distance, similarity) aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueMatching::isOk)
		.def_readonly("isError", &FSDKErrorValueMatching::isError)
		.def_readonly("FSDKError", &FSDKErrorValueMatching::fsdkError)
		.def_readonly("what", &FSDKErrorValueMatching::what)
		.def_readonly("value", &FSDKErrorValueMatching::value)
		.def("__repr__",
			[](const FSDKErrorValueMatching &err) {
				return "FSDKErrorValueMatching: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.fsdkError))
						+ ", value: (distance = " + std::to_string(err.value.distance) +
						", similarity = " + std::to_string(err.value.similarity) + ")"
						+ ", what = " + err.what;
			})
		;
	
	py::enum_<fsdk::ISettingsProvider::Error>(f, "SettingsProviderError",
		"Config parsing error codes.\n")
		.value("Ok", fsdk::ISettingsProvider::Error::Ok)
		.value("IOError", fsdk::ISettingsProvider::Error::IOError)
		.value("Memory", fsdk::ISettingsProvider::Error::Memory)
		.value("Internal", fsdk::ISettingsProvider::Error::Internal)
		.value("InvalidPi", fsdk::ISettingsProvider::Error::InvalidPi)
		.value("InvalidTag", fsdk::ISettingsProvider::Error::InvalidTag)
		.value("InvalidCdata", fsdk::ISettingsProvider::Error::InvalidCdata)
		.value("FileNotFound", fsdk::ISettingsProvider::Error::FileNotFound)
		.value("InvalidPcdata", fsdk::ISettingsProvider::Error::InvalidPcdata)
		.value("InvalidDocType", fsdk::ISettingsProvider::Error::InvalidDocType)
		.value("InvalidSettings", fsdk::ISettingsProvider::Error::InvalidSettings)
		.value("InvalidComment", fsdk::ISettingsProvider::Error::InvalidComment)
		.value("InvalidAttribute", fsdk::ISettingsProvider::Error::InvalidAttribute)
		.value("InvalidEndElement", fsdk::ISettingsProvider::Error::InvalidEndElement)
		.value("AppendInvalidRoot", fsdk::ISettingsProvider::Error::AppendInvalidRoot)
		.value("NoDocumentElement", fsdk::ISettingsProvider::Error::NoDocumentElement)
		.value("EndElementMismatch", fsdk::ISettingsProvider::Error::EndElementMismatch)
		.value("InvalidStartElement", fsdk::ISettingsProvider::Error::InvalidStartElement)
			;

	py::enum_<fsdk::DetectionComparerType>(f, "DetectionComparerType", py::arithmetic(),
			"Strategy of BestDetections comparer\n")
		.value("DCT_CONFIDANCE", fsdk::DCT_CONFIDANCE,
			"BestDetection - detections with highest score\n")
		.value("DCT_CENTER", fsdk::DCT_CENTER,
			"BestDetection - most centered detection\n")
		.value("DCT_CENTER_AND_CONFIDANCE", fsdk::DCT_CENTER_AND_CONFIDANCE,
			"BestDetection - most centered with high score\n")
		.export_values();
			;
	
	py::enum_<fsdk::DetectionType>(f, "DetectionType", py::arithmetic(), "Detection type type enumeration.\n")
		.value("dtBBox", fsdk::dtBBox, "Get bounding boxes of faces\n")
		.value("dt5Landmarks", fsdk::dt5Landmarks, "Get 5 facial landmarks\n")
		.value("dt68Landmarks", fsdk::dt68Landmarks, "Get 68 facial landmarks\n")
		.value("dtAll", fsdk::dtAll, "Get all supported parameters.\n")
		.export_values();
			;
	
	
	py::enum_<fsdk::FSDKError>(f, "FSDKError", "Common SDK error codes.\n")
		.value("Ok", fsdk::FSDKError::Ok)
		.value("Internal", fsdk::FSDKError::Internal)
		.value("InvalidInput", fsdk::FSDKError::InvalidInput)
		.value("InvalidImage", fsdk::FSDKError::InvalidImage)
		
		.value("InvalidRect", fsdk::FSDKError::InvalidRect)
		.value("InvalidImageFormat", fsdk::FSDKError::InvalidImageFormat)
		.value("InvalidImageSize", fsdk::FSDKError::InvalidImageSize)
		.value("InvalidDetection", fsdk::FSDKError::InvalidDetection)
		.value("InvalidLandmarks5", fsdk::FSDKError::InvalidLandmarks5)
		.value("InvalidLandmarks68", fsdk::FSDKError::InvalidLandmarks68)
		.value("InvalidTransformation", fsdk::FSDKError::InvalidTransformation)
		.value("InvalidDescriptor", fsdk::FSDKError::InvalidDescriptor)
		.value("InvalidDescriptorBatch", fsdk::FSDKError::InvalidDescriptorBatch)
		.value("InvalidSettingsProvider", fsdk::FSDKError::InvalidSettingsProvider)
		
		.value("ModuleNotInitialized", fsdk::FSDKError::ModuleNotInitialized)
		.value("ModuleNotReady", fsdk::FSDKError::ModuleNotReady)
		.value("LicenseError", fsdk::FSDKError::LicenseError)
		
		.value("BufferIsNull", fsdk::FSDKError::BufferIsNull)
		.value("BufferIsFull", fsdk::FSDKError::BufferIsFull)
		.value("BufferIsEmpty", fsdk::FSDKError::BufferIsEmpty)
		.value("InvalidBufferSize", fsdk::FSDKError::InvalidBufferSize)
		
		.value("IncompatibleDescriptors", fsdk::FSDKError::IncompatibleDescriptors)
			;

	py::enum_<fsdk::LicenseFeature>(f, "LicenseFeature", "License features.\n")
		.value("Detection", fsdk::LicenseFeature::Detection)
		.value("BestShot", fsdk::LicenseFeature::BestShot)
		.value("Attributes", fsdk::LicenseFeature::Attributes)
		.value("Emotions", fsdk::LicenseFeature::Emotions)
		.value("FaceFeatures", fsdk::LicenseFeature::FaceFeatures)
		.value("Liveness", fsdk::LicenseFeature::Liveness)
		.value("Descriptor", fsdk::LicenseFeature::Descriptor)
		.value("DescriptorIndex", fsdk::LicenseFeature::DescriptorIndex)
		.value("LivenessEngine", fsdk::LicenseFeature::LivenessEngine)
		.value("TrackEngine", fsdk::LicenseFeature::TrackEngine)
		.value("HumanDetection", fsdk::LicenseFeature::HumanDetection)
			;

	py::class_<fsdk::ILicensePtr>(f, "ILicensePtr",
		"License objects interface.\n"
		"Use License objects to adopt FaceEngine functionality.\n")
		.def("checkFeatureId", [](
			const fsdk::ILicensePtr& license,
			uint32_t featureId
			) {
				return license->checkFeatureId(featureId);
			},
			"Checks if the feature with featureId is available in this license.\n"
			"\t\t(see fsdk::LicenseFeature for details\n"
			"\tArgs:\n"
			"\t\tparam1 (featureId): featureId to check if it available\n"
			"\tReturns:\n"
			"\t\t(bool): True if feature is available, False if there is not such feature in this"
			"\t\tlicense or feature is expired or license was not activated.\n")

		.def("isActivated", [](
			const fsdk::ILicensePtr& license) {
				return license->isActivated();
			},
			"Checks if current license object is activated and could be used by FaceEngine."
			"License object which was not activated could not be used because all features are disabled by default.\n"
			"\tReturns:\n"
			"\t\tTrue if object is activated, False otherwise.\n")

		.def("loadFromFile", [](
			const fsdk::ILicensePtr& license,
			const char * path) {
				return license->loadFromFile(path);
			},
			"Loads license from file.\n"
			"\tArgs:\n"
			"\t\tparam1 (str) path to the file.\n"
			"\tReturns:\n"
			"\t\t(bool): True if license was read from file successfully, False otherwise.\n")

		.def("saveToFile", [](
			const fsdk::ILicensePtr& license,
			const char * path) {
				return license->saveToFile(path);
			},
			"Saves license as raw format to the file. This file could be used in the next run of the application.\n"
			"\t\tparam1 (str) path to the file.\n"
			"\tReturns:\n"
			"\t\t(bool): True if license was saved to file successfully, False otherwise.\n")
			;

	f.doc() = R"pbdoc(

        Python wrapper for LUNA SDK usings pybind11
        -------------------------------------------

        .. currentmodule:: FaceEngine

        .. autosummary::
           :toctree: _generate

			createFaceEngine
			createSettingsProvider
 			loadImage
 			Face
			FaceEngineEdition
			FaceEngineEdition.FrontEndEdition
			FaceEngineEdition.CompleteEdition

			PyIFaceEngine
			PyIFaceEngine.getFaceEngineEdition
			PyIFaceEngine.createAttributeEstimator
			PyIFaceEngine.createQualityEstimator
			PyIFaceEngine.createEthnicityEstimator

			PyIFaceEngine.createHeadPoseEstimator
			PyIFaceEngine.createBlackWhiteEstimator
			PyIFaceEngine.createDepthEstimator
			PyIFaceEngine.createIREstimator
			PyIFaceEngine.createSmileEstimator
			PyIFaceEngine.createFaceFlowEstimator
			PyIFaceEngine.createEyeEstimator
			PyIFaceEngine.createEmotionsEstimator
			PyIFaceEngine.createGazeEstimator
			PyIFaceEngine.createGlassesEstimator

			PyIFaceEngine.createDetector
			PyIFaceEngine.createWarper
			PyIFaceEngine.createDescriptor
			PyIFaceEngine.createDescriptorBatch
			PyIFaceEngine.createExtractor
			PyIFaceEngine.createMatcher
			PyIFaceEngine.setSettingsProvider
			PyIFaceEngine.createIndexBuilder
			PyIFaceEngine.loadDenseIndex
			PyIFaceEngine.loadDynamicIndex

			SettingsProviderValue
			SettingsProviderValue.__init__

			PyISettingsProvider.getDefaultPath
			PyISettingsProvider.load
			PyISettingsProvider.save
			PyISettingsProvider.clear
			PyISettingsProvider.isEmpty
			PyISettingsProvider.setValue
			PyISettingsProvider.getValue

			IQualityEstimatorPtr
			IQualityEstimatorPtr.estimate

			IAttributeEstimatorPtr
			IAttributeEstimatorPtr.estimate

			IGlassesEstimatorPtr
			IGlassesEstimatorPtr.estimate

			IEthnicityEstimator
			IEthnicityEstimator.estimate

			IDetectorPtr
			IDetectorPtr.detect
			IDetectorPtr.detectOne
			IDetectorPtr.setDetectionComparer

			IDetectorPtr.redetectOne
			IDetectorPtr.redetect
			Face
			Face.isValid

			IHumanDetectorPtr
			IHumanDetectorPtr.detect

			IWarperPtr
			IWarperPtr.warp
			IWarperPtr.createTransformation

			IDescriptorPtr
			IDescriptorPtr.getModelVersion
			IDescriptorPtr.getDescriptorLength
			IDescriptorPtr.getDescriptor
			IDescriptorPtr.getData
			IDescriptorPtr.load
			IDescriptorPtr.save

			IDescriptorBatchPtr
			IDescriptorBatchPtr.add
			IDescriptorBatchPtr.removeFast
			IDescriptorBatchPtr.removeSlow
			IDescriptorBatchPtr.getMaxCount
			IDescriptorBatchPtr.getCount
			IDescriptorBatchPtr.getModelVersion
			IDescriptorBatchPtr.getDescriptorSize
			IDescriptorBatchPtr.getDescriptorSlow
			IDescriptorBatchPtr.getDescriptorFast
			IDescriptorBatchPtr.load
			IDescriptorBatchPtr.save

			DescriptorBatchError

			IDescriptorExtractorPtr
			IDescriptorExtractorPtr.extract
			IDescriptorExtractorPtr.extractFromWarpedImage
			IDescriptorExtractorPtr.extractFromWarpedImageBatch


			IDescriptorMatcherPtr
			IDescriptorMatcherPtr.match

			IHeadPoseEstimatorPtr
			IHeadPoseEstimatorPtr.estimate

			IBlackWhiteEstimatorPtr
			IBlackWhiteEstimatorPtr.estimate

			ILivenessDepthEstimatorPtr
			ILivenessDepthEstimatorPtr.estimate
			ILivenessDepthEstimatorPtr.setRange

			ILivenessIREstimatorPtr
			ILivenessIREstimatorPtr.estimate

			ISmileEstimatorPtr
			ISmileEstimatorPtr.estimate

			ILivenessFlowEstimatorPtr
			ILivenessFlowEstimatorPtr.estimate

			IEyeEstimatorPtr
			IEyeEstimatorPtr.estimate
			
			EyesRects
			EyesRects.leftEyeRect
			EyesRects.leftEyeRect
			EyeCropper
			EyeCropper.__init__
			EyeCropper.cropByLandmarks5
			EyeCropper.cropByLandmarks68

			IEmotionsEstimatorPtr
			IEmotionsEstimatorPtr.estimate

			IGazeEstimatorPtr
			IGazeEstimatorPtr.estimate

			IAGSEstimatorPtr
			IAGSEstimatorPtr.estimate

			IIndexPtr
			IIndexPtr.search

			IDenseIndexPtr
			IDenseIndexPtr.search
			IDenseIndexPtr.size
			IDenseIndexPtr.descriptorByIndex
			IDenseIndexPtr.search

			IDynamicIndexPtr
			IDynamicIndexPtr.saveToDenseIndex
			IDynamicIndexPtr.saveToDynamicIndex
			IDynamicIndexPtr.search
			IDynamicIndexPtr.size
			IDynamicIndexPtr.descriptorByIndex
			IDynamicIndexPtr.appendDescriptor
			IDynamicIndexPtr.appendBatch
			IDynamicIndexPtr.removeDescriptor

			MatchingResult

			Landmarks5
			Landmarks5.__len__
			Landmarks5.__getitem__
			Landmarks5.__setitem__

			Landmarks68
			Landmarks68.__len__
			Landmarks68.__getitem__
			Landmarks68.__setitem__

			IrisLandmarks
			IrisLandmarks.__len__
			IrisLandmarks.__getitem__
			IrisLandmarks.__setitem__

			EyelidLandmarks
			EyelidLandmarks.__len__
			EyelidLandmarks.__getitem__
			EyelidLandmarks.__setitem__

			Vector2f
			Vector2f.__init__
			Vector2f.__repr__

			Vector2i
			Vector2i.__init__
			Vector2i.__repr__

			FSDKErrorResult
			DescriptorBatchResult
			ImageErrorResult
			SettingsProviderErrorResult
			FSDKErrorValueInt
			FSDKErrorValueFloat
			FSDKErrorValueMatching

			AttributeRequest
			AttributeRequest.estimateAge
			AttributeRequest.estimateGender
			AttributeRequest.estimateEthnicity

			AttributeResult
			AttributeResult.__init__
			AttributeResult.__repr__

			Quality
			Quality.__init__
			Quality.getQuality

			EthnicityEstimation
			EthnicityEstimation.__init__
			EthnicityEstimation.__repr__
			EthnicityEstimation.getEthnicityScore
			EthnicityEstimation.getPredominantEthnicity

			HeadPoseEstimation
			HeadPoseEstimation.__init__
			HeadPoseEstimation.__repr__
			HeadPoseEstimation.getFrontalFaceType

			SmileEstimation
			SmileEstimation.__init__
			SmileEstimation.__repr__

			EyesEstimation
			EyesEstimation.__init__
			EyesEstimation.__repr__
			State

			EyeAttributes
			EyeAttributes.state
			EyeAttributes.iris
			EyeAttributes.eyelid

			EmotionsEstimation
			EmotionsEstimation.__init__
			EmotionsEstimation.__repr__
			EmotionsEstimation.getPredominantEmotion
			EmotionsEstimation.getEmotionScore

			Emotions

			GazeEstimation
			GazeEstimation.__init__
			GazeEstimation.__repr__
			GazeEstimation

			EyeAngles

			Ethnicity

			GlassesEstimation

			Transformation

			Detection

			FormatType

			Image
			Image.__init__
			Image.getWidth
			Image.getHeight
			Image.isValid
			Image.getFormat
			Image.getRect
			Image.getData
			Image.getChannelCount
			Image.getChannelStep
			Image.getBitDepth
			Image.getByteDepth
			Image.computePitch
			Image.isPadded
			Image.isBGR
			Image.isValidFormat
			Image.setData
			Image.save
			Image.load
			Image.getRect
			Image.loadFromMemory

			ImageType

			ImageError
			ImageCompression

			SettingsProviderError

			Rect
			RectFloat

			ObjectDetectorClassType
			ObjectDetectorClassType.FACE_DET_DEFAULT
			ObjectDetectorClassType.FACE_DET_V1
			ObjectDetectorClassType.FACE_DET_V2
			ObjectDetectorClassType.FACE_DET_V3
			
			Detection
			Detection.isValid
			Detection.__repr__

			DetectionFloat
			DetectionFloat.isValid
			DetectionFloat.__repr__
			
			DetectionComparerType
			DetectionComparerType.DCT_CONFIDANCE
			DetectionComparerType.DCT_CENTER
			DetectionComparerType.DCT_CENTER_AND_CONFIDANCE

			DetectionType
			DetectionType.dtBBox
			DetectionType.dt5Landmarks
			DetectionType.dt68Landmarks

			FSDKError
			FSDKError.Ok
			FSDKError.Internal
			FSDKError.InvalidInput
			FSDKError.InvalidImage
			FSDKError.InvalidRect
			FSDKError.InvalidImageFormat
			FSDKError.InvalidImageSize
			FSDKError.InvalidDetection
			FSDKError.InvalidLandmarks5
			FSDKError.InvalidLandmarks68
			FSDKError.InvalidTransformation
			FSDKError.InvalidDescriptor
			FSDKError.InvalidDescriptorBatch
			FSDKError.InvalidSettingsProvider
			FSDKError.ModuleNotInitialized
			FSDKError.ModuleNotReady
			FSDKError.LicenseError
			FSDKError.BufferIsNull
			FSDKError.BufferIsFull
			FSDKError.BufferIsEmpty
			FSDKError.InvalidBufferSize
			FrontalFaceType
			FrontalFaceType.FrontalFace0
			FrontalFaceType.FrontalFace1
			FrontalFaceType.FrontalFace2

			DepthRange
			DepthRange.__repr__
			loadImage
			EyeAngles

			IIndexPtr
			IIndexPtr.search
			
			IDenseIndexPtr
			IDenseIndexPtr.search
			IDenseIndexPtr.size
			IDenseIndexPtr.descriptorByIndex
			
			IDynamicIndexPtr
			IDynamicIndexPtr.saveToDenseIndex
			IDynamicIndexPtr.saveToDynamicIndex
			IDynamicIndexPtr.countOfIndexedDescriptors
			IDynamicIndexPtr.search
			IDynamicIndexPtr.size
			IDynamicIndexPtr.descriptorByIndex
			IDynamicIndexPtr.search
			IDynamicIndexPtr.appendDescriptor
			IDynamicIndexPtr.appendBatch
			IDynamicIndexPtr.removeDescriptor
			
			IIndexBuilderPtr
			IIndexBuilderPtr.buildIndex
			IIndexBuilderPtr.appendDescriptor
			IIndexBuilderPtr.appendBatch
			IIndexBuilderPtr.appendDescriptor
			IIndexBuilderPtr.removeDescriptor
			IIndexBuilderPtr.descriptorByIndex

			LicenseFeature
			LicenseFeature.Detection
			LicenseFeature.BestShot
			LicenseFeature.Attributes
			LicenseFeature.Emotions
			LicenseFeature.FaceFeatures
			LicenseFeature.Liveness
			LicenseFeature.Descriptor
			LicenseFeature.DescriptorIndex
			LicenseFeature.LivenessEngine
			LicenseFeature.TrackEngine
			LicenseFeature.HumanDetection

			ILicensePtr
			ILicensePtr.checkFeatureId
			ILicensePtr.isActivated
			ILicensePtr.loadFromFile
			ILicensePtr.saveToFile

    )pbdoc";
}


