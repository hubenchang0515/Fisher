#ifndef FISHER_QRCODE_H
#define FISHER_QRCODE_H

#include <qrencode.h>
#include <string>

namespace Fisher
{

using std::string;

class QRCode
{
public:
    static QRCode create(const string& content) noexcept;

    QRCode(const QRCode& src) = delete;
    QRCode(QRCode&& src) noexcept;
    ~QRCode() noexcept;

    int width() const noexcept;
    const uint8_t* data() const noexcept;

private:
    QRcode* m_qrcode;

    QRCode() noexcept = default;
};

}; // namespace Fisher

#endif // FISHER_QRCODE_H