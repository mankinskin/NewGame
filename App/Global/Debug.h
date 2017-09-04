#pragma once
#include <string>
#include <vector>
namespace App {
	namespace Debug {
		struct Error {
			enum Severity {
				Warning,
				Trivial,
				Fatal
			}severity;

			Error(std::string pMsg, Severity pSev) {
				msg = pMsg;
				severity = pSev;
			}
			std::string msg = "";
		};

		void pushError(std::string pMessage, Error::Severity errorSeverity = Error::Severity::Warning);
		void printErrors();
		void togglePrintInfo();
		void printInfo();
		extern int shouldPrintInfo;
		extern std::vector<Error> errorBuffer;
	}
}
