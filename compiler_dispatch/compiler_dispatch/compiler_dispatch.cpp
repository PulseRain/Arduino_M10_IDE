#include <iostream>
#include <string>

constexpr std::size_t MAX_PATH_LENGTH = 8192;

bool string_start_with(std::string str_in, std::string str_match)
{
	if (str_in.length() < str_match.length()) {
		return false;
	}
	else {
		return (0 == str_in.compare(0, str_match.length(), str_match));
	}
}

bool string_end_with(std::string str_in, std::string str_match)
{
	if (str_in.length() < str_match.length()) {
		return false;
	} else {
		return (0 == str_in.compare(str_in.length() - str_match.length(), str_match.length(), str_match));
	}
}

std::string tail_replace(std::string str_in)
{
	std::string str_obj{ ".o" };
	std::string str_obj_quotation{ ".o\"" };

	std::string str_lib{ ".a" };
	std::string str_lib_quotation{ ".a\"" };

	std::string str_elf{ ".elf" };
	std::string str_elf_quotation{ ".elf\"" };


	std::string str_ret{str_in};

	if (string_end_with(str_in, str_obj)) {
		str_ret = str_in.substr(0, str_in.length() - str_obj.length()) + ".rel";
	} else if (string_end_with(str_in, str_obj_quotation)) {
		str_ret = str_in.substr(0, str_in.length() - str_obj_quotation.length()) + ".rel\"";
	} else if (string_end_with(str_in, str_lib)) {
		str_ret = str_in.substr(0, str_in.length() - str_lib.length()) + ".lib";
	} else if (string_end_with(str_in, str_lib_quotation)) {
		str_ret = str_in.substr(0, str_in.length() - str_lib_quotation.length()) + ".lib\"";
	} else if (string_end_with(str_in, str_elf)) {
		str_ret = str_in.substr(0, str_in.length() - str_elf.length()) + ".ihx";
	} else if (string_end_with(str_in, str_elf_quotation)) {
		str_ret = str_in.substr(0, str_in.length() - str_elf_quotation.length()) + ".ihx\"";
	}

	return str_ret;
}


std::string option_filter(std::string str_in)
{
	std::string str_ret{ str_in };


	if (string_start_with(str_in, "-mprocessor")) {
		str_ret = "";
	} else if (string_start_with(str_in, "-T")) {
		str_ret = "";
	} else if (string_end_with(str_in, "cpp-startup.S")) {
		str_ret = "";
	} else if (string_start_with(str_in, "-lm")) {
		str_ret = "";
	}
	return str_ret;
}

int main(int argc, char** argv)
{
	std::string preprocess_option { "-E" };
	std::string build_script_option{ "-T" };
	std::string dash_c_option{ "-c" };

	//std::string current_dir{ getexepath() };
	std::string str_argv0{argv[0]};

	std::string str_path{ "." };
	std::string str_cmd{ "" };
	std::string str_file{ "" };

	int preprocess_flag = 0;
	int dash_c_flag = 0;

	int ret;

	std::string cmd_option{ "" };

	for (int count = 1; count < argc; ++count) {
		//std::cout << count << " " << argv[count] << '\n';
		
		if (build_script_option.compare(argv[count - 1]) == 0) {
			continue;
		}
		
		std::string str_argv_tmp{ option_filter(argv[count]) };

		std::string inc_prefix{ "-I" };

		std::size_t space_found = str_argv_tmp.find_last_of(" ");


		if (space_found < MAX_PATH_LENGTH) {
			cmd_option += " \"" + tail_replace(str_argv_tmp)  + "\"";
		} else {
			cmd_option += " " + tail_replace(str_argv_tmp);
		}

		
		if (preprocess_option.compare(argv[count]) == 0) {
			preprocess_flag = 1;
		}

		if (dash_c_option.compare(argv[count]) == 0) {
			dash_c_flag = 1;
		}

	}
	
	std::size_t found = str_argv0.find_last_of("/\\");

	if (found < MAX_PATH_LENGTH) {
		str_path = str_argv0.substr(0, found);
		str_file = str_argv0.substr(found + 1);
	}
	else {
		str_file = str_argv0;
	}


	if (preprocess_flag) {
		str_cmd = "\"\"" + str_path + "\\..\\..\\avr\\bin\\avr-g++.exe" + "\""  + cmd_option + "\"" ;
	} else if (dash_c_flag) {
		str_cmd = "\"\"" + str_path + "\\sdcc.exe" + "\""  + " --disable-warning 151 -V --model-large --iram-size 128 --stack-loc 126 --nooverlay --xstack --std-c11 --nogcse --noinduction " + cmd_option + "\"" ;
	} else {
		str_cmd = "\"\"" + str_path + "\\sdcc.exe" + "\"" + " --disable-warning 151 -V --model-large --iram-size 128  --stack-loc 126 --nooverlay --xstack --std-c11 --nogcse --noinduction " + cmd_option + 
			
			"\"" + str_path + "\\..\\core\\main.c" 

			+ "\"\"";
	}
	 
//	std::cout << "cmd: " << str_cmd << "\n";
//	std::cerr << "cmd: " << str_cmd << "\n";

	ret = system(str_cmd.c_str()); 
//	ret = 0;
	
//	std::cerr << "ret = " << ret << "\n";

//	std::cin >> c;

	return ret;

}