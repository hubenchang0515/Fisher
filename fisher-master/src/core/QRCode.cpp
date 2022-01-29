#include <QRCode.h>

namespace Fisher
{

QRCode::QRCode(QRCode&& src) noexcept
{
    m_qrcode = src.m_qrcode;
    src.m_qrcode = nullptr;
}

QRCode::~QRCode()
{
    if (m_qrcode != nullptr)
        QRcode_free(m_qrcode);
}

int QRCode::width() const noexcept
{
    if (m_qrcode != nullptr)
        return m_qrcode->width;

    return 0;
}

const uint8_t* QRCode::data() const noexcept
{
    if (m_qrcode != nullptr)
        return m_qrcode->data;

    return nullptr;
}

// static
QRCode QRCode::create(const string& content) noexcept
{
    QRCode code;
    code.m_qrcode = QRcode_encodeString(content.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
    return code;
}

}; // namespace Fisher