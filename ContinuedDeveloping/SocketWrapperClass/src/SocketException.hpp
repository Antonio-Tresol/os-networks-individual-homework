/**
 * @file socket_exception.hpp
 * @brief Defines the SocketException class, which represents an exception 
 * thrown when a socket operation fails.
 */
#ifndef SOCKET_EXCEPTION_HPP
#define SOCKET_EXCEPTION_HPP

#include <exception>
#include <string>
#include <cstring>
#include <cerrno>

/**
 * @class SocketException
 * @brief Exception thrown when a socket operation fails
 * @details This exception is thrown when a socket operation (e.g. connect, 
 * bind, send, etc.) fails. It stores the error message, the name of the 
 * function that failed, and the error code associated with the failure.
 */
class SocketException : public std::exception {
 public:
  /**
   * @brief Constructor for SocketException
   * @param message The error message to associate with the exception
   * @param function The name of the function that failed
   * @param errorCode The error code associated with the failure
   */
  SocketException(const std::string& message, const std::string& function, 
    int errorCode);

  /**
   * @brief Returns a C-style character string describing the exception
   * @return A C-style character string describing the exception
   */
  virtual const char* what() const noexcept override;

  /**
   * @brief Returns the name of the function that failed
   * @return The name of the function that failed
   */
  const std::string& function() const noexcept;

  /**
   * @brief Returns the error code associated with the failure
   * @return The error code associated with the failure
   */
  int errorCode() const noexcept;

  /**
   * @brief Returns a string describing the error
   * @return A string describing the error
   */
  std::string errorMessage() const noexcept;

 private:
  std::string m_message; ///< The error message associated with the exception
  std::string m_function; ///< The name of the function that failed
  int m_errorCode; ///< The error code associated with the failure
};

#endif // SOCKET_EXCEPTION_HPP
