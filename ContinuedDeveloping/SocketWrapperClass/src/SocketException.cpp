#include "SocketException.hpp"

SocketException::SocketException(const std::string& message,
  const std::string& function, int errorCode)
  : mMessage(message), mFunction(function), mErrorCode(errorCode) {
      this->mErrorMessage = errorMessage();
  }

SocketException::SocketException(const std::string& message, 
  const std::string& function, bool isSslError)
  : mMessage(message), mFunction(function) {
  if (isSslError) {
    this->mErrorMessage = sslError(mMessage, mFunction);
  } else {
    this->mErrorMessage = mMessage + " (" + mFunction + ")";
  }
}
const char* SocketException::what() const noexcept {
  return this->mErrorMessage.c_str();
}

const std::string& SocketException::function() const noexcept {
  return mFunction;
}

int SocketException::errorCode() const noexcept {
  return mErrorCode;
}

std::string SocketException::errorMessage() const noexcept {
  return mMessage + " (" + mFunction + ") - " + strerror(mErrorCode);
}

std::string SocketException::sslError(const std::string& message,
  const std::string& function) const noexcept {
  uint64_t error = ERR_get_error();
  const char* buffer = ERR_reason_error_string(error);
  std::string sslErrorMessage = message + " (" + function + ") - ";
  if (buffer) {
      sslErrorMessage += buffer;
  } else {
      sslErrorMessage += "NULL";
  }
  return sslErrorMessage;
}