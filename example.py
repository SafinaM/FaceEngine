import sys

def help():
    print("python example.py <path to FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as f

image_path = sys.argv[2]


# correct paths to data or put directories data and testData with example.py
faceEnginePtr = f.createFaceEngine("data",
                                      "data/faceengine.conf")

attributeEstimator = faceEnginePtr.createAttributeEstimator()
qualityEstimator = faceEnginePtr.createQualityEstimator()
ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()

print(attributeEstimator)
print(qualityEstimator)
print(ethnicityEstimator)

# image downloading example
print("image path: {0}".format(image_path))
image = f.Image()
print(f.FormatType.R8)
print(f.Format())
err = image.load(image_path)
print("image was downloaded {0}".format(err.isOk))
print(image.load(image_path, f.Format(f.FormatType.R8G8B8)))
print(image)
print("image width {0}".format(image.getWidth()))
print("image height {0}".format(image.getHeight()))
print("image is valid {0}".format(image.isValid()))

attribute_result = attributeEstimator.estimate(image)
quality_result = qualityEstimator.estimate(image)
ethnicity_result = ethnicityEstimator.estimate(image)

print("Detector example")
detector = faceEnginePtr.createDetector(f.ODT_MTCNN)
print("Ethnicity result: ", ethnicity_result)
print("test of getEthnicityScore() {0}".format(ethnicity_result.getEthnicityScore(f.Ethnicity.Caucasian)))
print("test of get getPredominantEthnicity() {0}".format(ethnicity_result.getPredominantEthnicity()))

print("Attribute result", attribute_result)
print(quality_result)
print(ethnicity_result)

# detector test and example
print("detector test")
maxDetections = 3
image_det = f.Image()
err = image_det.load(image_path)
print(image.getHeight(), image.getWidth(), image.isValid())
print("Image error = ", err)

detector = faceEnginePtr.createDetector(f.ODT_MTCNN)

detector_result = detector.detect(image_det, image_det.getRect(), maxDetections)
print("detector result = ", detector_result)
test = detector_result[0][1]
print("Landmarks test {0}".format(test[0]))
print("Landmarks test {0}".format(test[0]))

print("Detections: ")
for i, item in enumerate(detector_result, 1):
    print(i, item)


# warper test and example
print("Warper example: ")
warper = faceEnginePtr.createWarper()
transformation = warper.createTransformation(detector_result[0][0],
                                             detector_result[0][1])
print("transformation = ", transformation)
warperResult = warper.warp(image_det, transformation)
warpImage = warperResult
print(warpImage)
print(warpImage.getWidth(), warpImage.getHeight(), warpImage.isValid())
ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
estimation = ethnicityEstimator.estimate(warpImage)
print("Ethnicity estimator on warped Image", estimation)

transformedLandmarks5 = warper.warp(detector_result[0][1], transformation)
print("Gaze test")
for i in range(len(transformedLandmarks5)):
    print(transformedLandmarks5[i])
transformedLandmarks68 = warper.warp(detector_result[0][2], transformation)

print("warperResult2 with Landmarks5 = ", transformedLandmarks5)
print("warperResult2 with Landmarks68 = ", transformedLandmarks68 )


# descriptor, creating objects
descriptor1 = faceEnginePtr.createDescriptor()
descriptor2 = faceEnginePtr.createDescriptor()
aggregation = faceEnginePtr.createDescriptor()

print("Batch descriptor example")
testData = [f.Image(), f.Image(), f.Image()]
# for i in range(2):
testData[0].load("testData/warp1.ppm")
testData[1].load("testData/warp2.ppm")
testData[2].load("testData/photo_2017-03-30_14-47-43_p.ppm")

if not testData[0].isValid() or not testData[1].isValid() or not testData[2].isValid():
    print("path to images for batch createing is not found")
    exit(1)

batchSize = len(testData)
descriptorBatch = faceEnginePtr.createDescriptorBatch(batchSize)
extractor = faceEnginePtr.createExtractor()
matcher = faceEnginePtr.createMatcher()
table = faceEnginePtr.createLSHTable(descriptorBatch)

print(testData)
print(type(extractor))
print("Descriptor test befor = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
ext1 = extractor.extractFromWarpedImage(testData[0], descriptor1)
ext2 = extractor.extractFromWarpedImage(testData[1], descriptor2)
print("Descriptor test after = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
print("extractor result =", ext2)

print("Descriptor batch test befor", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
ext_batch1 = extractor.extractFromWarpedImageBatch(testData, descriptorBatch, aggregation, batchSize)
# print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
ext_batch2 = extractor.extractFromWarpedImageBatch(testData, descriptorBatch, batchSize)

print("Garbage score list1 = ", ext_batch1)
print("Garbage score list2 = ", ext_batch2)
print("Descriptor batch test after", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
print(descriptor1)
print(descriptorBatch)
print(extractor)
print(matcher)
print(table)

# matcher test
print("Matcher example")
result1 = matcher.match(descriptor1, descriptor2)
result2 = matcher.match(descriptor1, descriptorBatch)
result3 = matcher.match(descriptor1, descriptorBatch, [0, 1])
result4 = matcher.matchCompact(descriptor1, descriptorBatch, [1])
print(result1)
print(result2)
print(result3)
print(result4)

print("Estimators example: ")
# test of second estimators part
headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
blackWhiteEstimator = faceEnginePtr.createBlackWhiteEstimator()
depthEstimator = faceEnginePtr.createDepthEstimator()
iREstimator = faceEnginePtr.createIREstimator()
smileEstimator = faceEnginePtr.createSmileEstimator()
faceFlowEstimator = faceEnginePtr.createFaceFlowEstimator()
eyeEstimator = faceEnginePtr.createEyeEstimator()
emotionsEstimator = faceEnginePtr.createEmotionsEstimator()
gazeEstimator = faceEnginePtr.createGazeEstimator()

print("Estimators objects: ")
print(headPoseEstimator)
print(blackWhiteEstimator)
print(depthEstimator)
print(iREstimator)
print(smileEstimator)
print(faceFlowEstimator)
print(eyeEstimator)
print(emotionsEstimator)
print(gazeEstimator)

# unpack detector result
(_, landmarks5, landmarks68) = detector_result[0]

# headPose
headPoseEstimation = headPoseEstimator.estimate(landmarks68)
for i in range(len(landmarks68)):
    print(landmarks68[i])
print(headPoseEstimator.estimate(landmarks68))
# blackWhite
print(blackWhiteEstimator.estimate(image))
# depth
depthImage = f.loadImage("testData/warp.depth")
print("Depth estimation result = {0}".format(depthEstimator.estimate(depthImage)))
# ir
irImage = f.Image()
irImage.load("testData/irWarp.ppm")
print("iRresult = ", iREstimator.estimate(irImage))
# smile
print(smileEstimator.estimate(image))


print("transformedLandmarks5[0]", transformedLandmarks5)
# faceFlow
print("FaceFlowExample")
faceFlowImage = f.Image()
faceFlowImage.load("testData/small.ppm")
sequence = []
for i in range(10):
    tempImage = f.Image()
    tempImage.load("testData/" + str(i) + "big.ppm")
    sequence.append(tempImage)

faceFlowResult = faceFlowEstimator.estimate(faceFlowImage, sequence, len(sequence))
print("faceFlowResult {0}".format(faceFlowResult))
# eyes
eyesEstimation = eyeEstimator.estimate(warpImage, transformedLandmarks5)

print(eyesEstimation.leftEye.state, eyesEstimation.leftEye.iris, eyesEstimation.leftEye.eyelid)
print(eyesEstimation.rightEye.state, eyesEstimation.rightEye.iris, eyesEstimation.rightEye.eyelid)
# emotions
print(emotionsEstimator.estimate(warpImage))

print(gazeEstimator.estimate(headPoseEstimation, eyesEstimation))

settingsProvider = f.createSettingsProvider("data/faceengine.conf")
path = settingsProvider.getDefaultPath()
print(path)


def are_equal(desc1, desc2):
    assert(len(desc1) == len(desc2))
    for i, _ in enumerate(desc1):
        if desc1[i] != desc2[i]:
            return False
    return True


def extractor_test_aggregation(version, use_mobile_net, cpu_type, device):
    print("extractor_test_aggregation")
    config = f.createSettingsProvider("data/faceengine.conf")
    configPath = settingsProvider.getDefaultPath()
    config.setValue("DescriptorFactory::Settings", "model", f.SettingsProviderValue(version))
    config.setValue("DescriptorFactory::Settings", "useMobileNet", f.SettingsProviderValue(use_mobile_net))
    config.setValue("flower", "deviceClass", f.SettingsProviderValue(device))
    config.setValue("system", "cpuClass", f.SettingsProviderValue(cpu_type))
    config.setValue("system", "verboseLogging", f.SettingsProviderValue(5))
    # config.setValue("QualityEstimator::Settings", "logGray", f.SettingsProviderValue(0.05, 3.3, 0.05, 0.012))
    faceEnginePtr.setSettingsProvider(config)
    # try to test settingsProvider
    val = config.getValue("QualityEstimator::Settings", "platt")
    print(val.asPoint2f())

    val = config.getValue("QualityEstimator::Settings", "expBlur")
    print(val.asPoint2f())
    print(val.asPoint2i())
    print(val.asSize())
    print(val.asRect())
    print(val.asBool())
    print(val.asInt())
    print(val.asFloat())
    print(val.asString())


    faceEnginePtr.setSettingsProvider(config)
    val = config.getValue("MTCNNDetector::Settings", "scaleFactor")
    print(val.asFloat())

    # print("msd config test = {0}".format(s))

    warps = [f.Image(), f.Image()]

    warps[0].load("testData/warp1.ppm")
    warps[1].load("testData/warp2.ppm")
    batchSize = len(warps)
    descriptorExtractor = faceEnginePtr.createExtractor()
    batch = faceEnginePtr.createDescriptorBatch(batchSize)
    descriptor = faceEnginePtr.createDescriptor()
    descriptor2 = faceEnginePtr.createDescriptor()
    aggregation = faceEnginePtr.createDescriptor()

    result1 = descriptorExtractor.extractFromWarpedImageBatch(warps, batch, aggregation, batchSize)
    print(result1)
    print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
    result2 = descriptorExtractor.extractFromWarpedImage(warps[0], descriptor)
    result3 = descriptorExtractor.extractFromWarpedImage(warps[1], descriptor2)
    # # test of error
    # print("Image is valid = {0}".format(warps[2].isValid()))
    # result4 = descriptorExtractor.extractFromWarpedImage(warps[2], descriptor2)
    # print("test of return error", result4)
    print(result2)
    print(result3)
    print(descriptor.getModelVersion() == batch.getModelVersion())
    print(batch.getMaxCount())
    print(batch.getCount())
    print(batch.getDescriptorSize())
    desc1 = descriptor.getDescriptor()
    desc2 = descriptor2.getDescriptor()

    desc_from_batch = batch.getDescriptorSlow(1).getDescriptor()
    for i, element in enumerate(desc1):
        print(i, ")", desc1[i], desc_from_batch[i])
    print("Descritptor comparing {0}".format(are_equal(desc1, desc_from_batch)))

extractor_test_aggregation(46, True, "cpu", "cpu")


