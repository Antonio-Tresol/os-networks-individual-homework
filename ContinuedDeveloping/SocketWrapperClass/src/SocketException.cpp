#include "SocketException.hpp"

SocketException::SocketException(const std::string& message,
    const std::string& function, int errorCode)
    : m_message(message), m_function(function), m_errorCode(errorCode) {
        this->m_message = errorMessage();
    }

const char* SocketException::what() const noexcept {
    return this->m_message.c_str();
}

const std::string& SocketException::function() const noexcept {
    return m_function;
}

int SocketException::errorCode() const noexcept {
    return m_errorCode;
}

std::string SocketException::errorMessage() const noexcept {
    return m_message + " (" + m_function + ") - " + strerror(m_errorCode);
}