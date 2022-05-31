#include "Connection.h"


void Connection::start() {
	auto strongThis = shared_from_this();

	size_t send = buffer.insert_hello_message(game.generate_Hello());

	_socket.async_send(
			boost::asio::buffer(buffer.get_send(), send),
			[strongThis](
					const boost::system::error_code &error,
					size_t bytesTransferred) {
				if (error) {
					std::cout << "Failed to send message!\n";
				} else {
					std::cout << "Sent Hello!\n";
				}
			});

	boost::asio::streambuf buffer_;

	_socket.async_receive(buffer_.prepare(512),
	                      [this](const boost::system::error_code &error,
	                             size_t bytesTransferred) {
		                      if (error == boost::asio::error::eof) {
			                      std::cout
					                      << "Client disconnected properly! \n";
		                      } else if (error) {
			                      std::cout
					                      << "Client disconnected in bad way! \n";
		                      }
	                      });
}