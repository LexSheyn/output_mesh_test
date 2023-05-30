#include "output_mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#define RESULT_ERROR   1
#define RESULT_SUCCESS 0

#define LOG_INFO  "info:\t "
#define LOG_ERROR "error:\t "

#define FILE_COMMENT   "*"
#define FILE_NODES     "Nodes"
#define FILE_ELEMENTS  "Elements"
#define FILE_NUMBER    'N'
#define FILE_X         'X'
#define FILE_Y         'Y'
#define FILE_Z         'Z'
#define FILE_DELIMITER ','

struct vec3
{
	float x;
	float y;
	float z;
};

int32_t output_mesh(int32_t argument_count, char** argument_values)
{
	// Parse arguments:

	const std::string token_i      = "-i";
	const std::string token_input  = "--input";
	const std::string token_o      = "-o";
	const std::string token_output = "--output";

	std::string input_file_name;
	std::string output_file_name;

	std::string argument;

	for (int32_t i = 1; i < argument_count; ++i)
	{
		argument = argument_values[i];

		if ((argument == token_i) || (argument == token_input))
		{
			input_file_name = argument_values[i + 1];

		//	std::cout << LOG_INFO << "input file name is - " << input_file_name << '\n';
		}
		else if ((argument == token_o) || (argument == token_output))
		{
			output_file_name = argument_values[i + 1];

		//	std::cout << LOG_INFO << "output file name is - " << output_file_name << '\n';
		}
	}

	if (input_file_name.empty())
	{
		std::cerr << LOG_ERROR << "input file name is not provided!" << std::endl;

		return RESULT_ERROR;
	}
	else if ((input_file_name == token_o) || (input_file_name == token_output))
	{
		std::cerr << LOG_ERROR << "input file name is missing!" << std::endl;

		return RESULT_ERROR;
	}

	if (output_file_name.empty())
	{
		std::cerr << LOG_ERROR << "output file name is not provided!" << std::endl;

		return RESULT_ERROR;
	}
	else if ((output_file_name == token_i) || (output_file_name == token_input))
	{
		std::cerr << LOG_ERROR << "output file name is missing!" << std::endl;

		return RESULT_ERROR;
	}

	// Input file:

	std::ifstream input_file_stream;

	std::stringstream input_file_string_stream;

	std::string line;
	std::string value;

	std::vector<vec3>   vertex_positions;
	std::vector<size_t> indices;

	vec3 temp_vertex_position = {};

	input_file_stream.open(input_file_name);

	if (!input_file_stream.is_open())
	{
		std::cerr << LOG_ERROR << "failed to open input file - " << input_file_name << std::endl;

		return RESULT_ERROR;
	}

	while (std::getline(input_file_stream, line))
	{
		input_file_string_stream.clear();
		input_file_string_stream.str(line);
		input_file_string_stream >> value;

		if (value == FILE_COMMENT)
		{
			continue;
		}
		else if (value.find_first_of("0123456789") != std::string::npos)
		{
			input_file_string_stream >> temp_vertex_position.x;
			input_file_string_stream.ignore(1);
			input_file_string_stream >> temp_vertex_position.y;
			input_file_string_stream.ignore(1);
			input_file_string_stream >> temp_vertex_position.z;

			vertex_positions.push_back(temp_vertex_position);
		}
	}

	input_file_stream.close();

	for (size_t i = 0u; i < vertex_positions.size() - 3; ++i)
	{
		indices.push_back(i + 0 + 1);
		indices.push_back(i + 1 + 1);
		indices.push_back(i + 2 + 1);
		indices.push_back(i + 0 + 1);
		indices.push_back(i + 2 + 1);
		indices.push_back(i + 3 + 1);
	}

	// Output file:

	std::ofstream output_file_stream;

	std::ostringstream output_file_string_stream;

	output_file_string_stream << FILE_COMMENT;
	output_file_string_stream << std::right << std::setw(4) << FILE_NUMBER << FILE_DELIMITER;
	output_file_string_stream << std::right << std::setw(13) << FILE_X << FILE_DELIMITER;
	output_file_string_stream << std::right << std::setw(13) << FILE_Y << FILE_DELIMITER;
	output_file_string_stream << std::right << std::setw(13) << FILE_Z;
	output_file_string_stream << '\n';
	output_file_string_stream << FILE_COMMENT " " FILE_NODES;
	output_file_string_stream << '\n';

	for (size_t i = 0; i < vertex_positions.size(); ++i)
	{
		output_file_string_stream << std::right << std::setw(5) << i + 1 << FILE_DELIMITER;
		output_file_string_stream << std::right << std::setw(13) << std::setprecision(8) << std::noshowpoint << vertex_positions[i].x << FILE_DELIMITER;
		output_file_string_stream << std::right << std::setw(13) << std::setprecision(8) << std::noshowpoint << vertex_positions[i].y << FILE_DELIMITER;
		output_file_string_stream << std::right << std::setw(13) << std::setprecision(8) << std::noshowpoint << vertex_positions[i].z;
		output_file_string_stream << '\n';
	}

	output_file_string_stream << FILE_COMMENT " " FILE_ELEMENTS;
	output_file_string_stream << '\n';

	size_t index_counter = 0;

	for (size_t i = 0; i < indices.size(); ++i)
	{
		if (index_counter == 0)
		{
			output_file_string_stream << std::right << std::setw(5) << i + 1 << FILE_DELIMITER;
		}

		output_file_string_stream << std::right << std::setw(13) << indices[i];

		index_counter++;

		if (index_counter == 3)
		{
			index_counter = 0;

			output_file_string_stream << '\n';
		}
		else
		{
			output_file_string_stream << FILE_DELIMITER;
		}
	}

	output_file_stream.open(output_file_name);

	if (!output_file_stream.is_open())
	{
		std::cerr << LOG_ERROR << "failed to open output file - " << output_file_name << std::endl;

		return RESULT_ERROR;
	}

	output_file_stream << output_file_string_stream.str();

	output_file_stream.close();

	return RESULT_SUCCESS;
}
