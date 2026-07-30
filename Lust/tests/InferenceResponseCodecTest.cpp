#include <QtTest>

#include "Protocol/InferenceResponseCodec.h"

class InferenceResponseCodecTest : public QObject
{
    Q_OBJECT

private slots:
    void resultRoundTripPreservesFields();
    void errorRoundTripPreservesFields();
    void invalidPayloadsAreRejected();
};


void InferenceResponseCodecTest::
    resultRoundTripPreservesFields()
{
    InferenceResponseCodec::InferenceResultPayload expected;
    expected.detectionLevel = QStringLiteral("NG");
    expected.score = 0.873;
    expected.inferenceMs = 18;
    expected.defectArea = 126;

    QString errorMessage;
    const QByteArray jsonBytes =
        InferenceResponseCodec::encodeResult(
            expected,
            &errorMessage);

    QVERIFY2(
        !jsonBytes.isEmpty(),
        qPrintable(errorMessage));

    InferenceResponseCodec::InferenceResultPayload actual;
    QVERIFY2(
        InferenceResponseCodec::decodeResult(
            jsonBytes,
            &actual,
            &errorMessage),
        qPrintable(errorMessage));
    QCOMPARE(
        actual.detectionLevel,
        expected.detectionLevel);
    QCOMPARE(actual.score, expected.score);
    QCOMPARE(actual.inferenceMs, expected.inferenceMs);
    QCOMPARE(actual.defectArea, expected.defectArea);
}


void InferenceResponseCodecTest::
    errorRoundTripPreservesFields()
{
    InferenceResponseCodec::InferenceErrorPayload expected;
    expected.errorCode =
        QStringLiteral("FRAME_DROPPED");
    expected.message =
        QStringLiteral("Replaced by a newer frame");

    QString errorMessage;
    const QByteArray jsonBytes =
        InferenceResponseCodec::encodeError(
            expected,
            &errorMessage);

    QVERIFY2(
        !jsonBytes.isEmpty(),
        qPrintable(errorMessage));

    InferenceResponseCodec::InferenceErrorPayload actual;
    QVERIFY2(
        InferenceResponseCodec::decodeError(
            jsonBytes,
            &actual,
            &errorMessage),
        qPrintable(errorMessage));
    QCOMPARE(actual.errorCode, expected.errorCode);
    QCOMPARE(actual.message, expected.message);
}


void InferenceResponseCodecTest::
    invalidPayloadsAreRejected()
{
    QString errorMessage;

    InferenceResponseCodec::InferenceResultPayload
        invalidResult;
    invalidResult.detectionLevel =
        QStringLiteral("UNKNOWN");
    QVERIFY(
        InferenceResponseCodec::encodeResult(
            invalidResult,
            &errorMessage)
            .isEmpty());
    QVERIFY(!errorMessage.isEmpty());

    InferenceResponseCodec::InferenceErrorPayload
        invalidError;
    invalidError.errorCode =
        QStringLiteral("INFERENCE_FAILED");
    QVERIFY(
        InferenceResponseCodec::encodeError(
            invalidError,
            &errorMessage)
            .isEmpty());
    QVERIFY(!errorMessage.isEmpty());

    InferenceResponseCodec::InferenceResultPayload
        decodedResult;
    QVERIFY(
        !InferenceResponseCodec::decodeResult(
            QByteArray("{not-json"),
            &decodedResult,
            &errorMessage));
    QVERIFY(!errorMessage.isEmpty());

    const QByteArray unsupportedResultSchema(
        R"({"schema_version":2,"detection_level":"OK","score":0.1,"inference_ms":1,"defect_area":0})");
    QVERIFY(
        !InferenceResponseCodec::decodeResult(
            unsupportedResultSchema,
            &decodedResult,
            &errorMessage));
    QVERIFY(!errorMessage.isEmpty());

    InferenceResponseCodec::InferenceErrorPayload
        decodedError;
    const QByteArray invalidErrorJson(
        R"({"schema_version":1,"error_code":"","message":"failed"})");
    QVERIFY(
        !InferenceResponseCodec::decodeError(
            invalidErrorJson,
            &decodedError,
            &errorMessage));
    QVERIFY(!errorMessage.isEmpty());
}


QTEST_APPLESS_MAIN(InferenceResponseCodecTest)

#include "InferenceResponseCodecTest.moc"
