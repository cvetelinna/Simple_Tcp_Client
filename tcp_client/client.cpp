#include <boost/asio.hpp> 
#include <fstream>
#include <iostream> 
#include <regex>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

string getData(tcp::socket& socket)
{
	boost::asio::streambuf buf;
	read_until(socket, buf, "\n");
	string data = buffer_cast<const char*>(buf.data());
	return data;
}

void sendData(tcp::socket& socket, const string& message)
{
	write(socket,
		buffer(message + "\r\n"));

}

bool isValidEmail(const string& email)
{
	const regex pattern("([a-z]+)([_.a-z0-9]*)([a-z0-9]+)(@)([a-z]+)([.a-z]+)([a-z]+)");
	return regex_match(email, pattern);
}

int main(int argc, char* argv[])
{
	io_service io_service;
	// socket creation 
	ip::tcp::socket client_socket(io_service);
	boost::system::error_code ec;
	client_socket
		.connect(
			tcp::endpoint(
				address::from_string("127.0.0.1"),
				587), ec);

	std::cout << ec.message();
	string domain, u_name, rcpt_name, msg, reply, response;
	response = getData(client_socket);

	cout << "HELO: ";
	getline(cin, domain);

	cout << "MAIL FROM: ";
	getline(cin, u_name);
	if (isValidEmail(u_name) == false)
	{
		cout << "Enter valid email address!" << endl;
		cin.clear();
		getline(cin, u_name);
	}

	cout << "MAIL TO: ";
	getline(cin, rcpt_name);

	//this is done in oder to send automatic message
	const char* fileName = "C:\\Users\\Username\\Desktop\\sometext.txt";

	ifstream file;
	string line;
	file.open(fileName, ios::out);
	if (!file)
	{
		cout << "Error in opening file!!!" << endl;
		return -1;
	}
	while (getline(file, line))
	{
		//ss << line << "\r\n";
		msg.append(line);
		msg.append("\r\n");
	}
	msg.append("\r\n");
	msg.append(".");

	file.close();

	sendData(client_socket, "HELO " + domain);
	response = getData(client_socket);
	cout << response << endl;

	sendData(client_socket, "MAIL FROM: " + u_name);
	response = getData(client_socket);
	cout << response << endl;

	sendData(client_socket, "RCPT TO: " + rcpt_name);
	response = getData(client_socket);
	cout << response << endl;

	sendData(client_socket, "DATA");
	response = getData(client_socket);
	cout << response << endl;

	sendData(client_socket, msg);
	response = getData(client_socket);
	cout << response << endl;

	while (1) {
		response = getData(client_socket);

		response.pop_back();

		if (response == "quit") {
			cout << "Connection terminated" << endl;
			break;
		}
		cout << "Server: " << response << endl;
		cout << u_name << " ";
		getline(cin, reply);
		sendData(client_socket, reply);

		if (reply == "quit")
		{
			break;
		}
	}
	return 0;
}