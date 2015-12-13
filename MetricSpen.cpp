// MetricSpen.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "const.h"
#include <windows.h>
#include <fstream>
#include <iostream>
#include <regex>

using namespace std;

string input;
string globalVariables[BORDER];
string localVariables[BORDER];
string functions[BORDER];
string funcHeaders[BORDER];
void SetTextColor(int textColor);
void findGlobalVariables(string inputText, string *variablesArray);
void findVariablesInFunctions(string functionText, string *variablesArray);
void processResults(string *variablesArray);
void readFromFile(string *inputText, string fileName);
void deleteComments();
void setFunctionsArray(string function);
int totalCounter, functionsCounter;


void main()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	csbi.dwSize.X = 600;
	csbi.dwSize.Y = 500;
	SetConsoleScreenBufferSize(hConsole, csbi.dwSize);

	setlocale(LC_ALL, "rus");
	readFromFile(&input, "source.php");

	SetTextColor(White);
	cout << "\n                                 Исходный код:\n         " << endl;
	SetTextColor(DarkGray);
	cout << input << endl;
	deleteComments(); 
	setFunctionsArray(input);

	SetTextColor(White);
	cout << "\n\n\n                                 Результат:\n         " << endl;
	SetTextColor(DarkGray);
	findGlobalVariables(input, globalVariables);
	processResults(globalVariables);

	for (int i = 0; i < functionsCounter; i++)
	{
		SetTextColor(Magenta);
		cout << funcHeaders[i] << endl;
		SetTextColor(DarkGray);
		findVariablesInFunctions(functions[i], localVariables);
		processResults(localVariables);
	}
	system("pause");
}

void readFromFile(string *inputText, string fileName)
{
	ifstream inputFile(fileName);
	if (inputFile.is_open())
	{
		SetTextColor(DarkGray);
		while (inputFile)
		{
			inputFile.unsetf(std::ios_base::skipws);
			std::copy(istream_iterator< char >(inputFile), istream_iterator< char >(), std::back_inserter(*inputText));
		}
		inputFile.close();
	}
	else
		cout << "Ошибка открытия файла!" << endl;
}

void deleteComments()
{
	regex commentPattern("[\/][\/].*|[\#].*|[\/][\*][^]*?[\*][\/]|[\'][^]*?[\']");
	smatch searchResults;
	input = regex_replace(input, commentPattern, "");
}

void findGlobalVariables(string inputText, string *variablesArray)
{
	regex variableRegex("[\$]+[\{]*[_\${А-Яа-яA-Za-z][А-Яа-яA-Za-z0-9_\}]*");
	smatch variables;
	int i = 0;
	while (regex_search(inputText, variables, variableRegex))
	{
		i++;
		for (auto output : variables)
			variablesArray[i - 1] = output;
		inputText = variables.suffix().str();
	}
	totalCounter = i;
}

void findVariablesInFunctions(string functionText, string *variablesArray)
{
	regex variableRegex("global[ ]*[\$]+[\{]*[_\${A-Za-zА-Яа-я][_A-Za-zА-Яа-я0-9\}]*|[\$]+[\{]*[_\${A-Za-zА-Яа-я][_A-Za-zА-Яа-я0-9\}]*");
	smatch variables;
	int i = 0;
	while (regex_search(functionText, variables, variableRegex))
	{
		i++;
		for (auto output : variables)
			variablesArray[i - 1] = output;
		functionText = variables.suffix().str();
	}
	totalCounter = i;
}

void setFunctionsArray(string function)
{
	string currentFunctionHeader;
	regex functionRegex("function [^]*?[\{]");
	smatch result;
	int i = 0;
	while (regex_search(function, result, functionRegex))
	{
		for (auto output : result)
			currentFunctionHeader = output;
		function = result.suffix().str();
		i++;
		int currentPosition = input.find(currentFunctionHeader);
		currentPosition = currentPosition + currentFunctionHeader.length();

		regex functionNameRegex("function[ ][_{A-Za-z][_A-Za-z0-9\}]*");
		smatch Header;
		regex_search(currentFunctionHeader, Header, functionNameRegex);
		for (auto output : Header)
			funcHeaders[i - 1] = output;

		int bracketsCounter = 0;
		int positionInFunction = 0;
		char functionText[BORDER] = "";
		do
		{
			switch (input[currentPosition])
			{
			case '{':
				bracketsCounter++;
				break;
			case '}':
				bracketsCounter--;
				break;
			}
			functionText[positionInFunction] = input[currentPosition];
			input[currentPosition] = ' ';
			positionInFunction++;
			currentPosition++;
		} while (bracketsCounter >= 0);
		functions[i - 1] = functionText;
	}
	functionsCounter = i;
}

void processResults(string *variablesArray)
{
	for (int i = 0; i < totalCounter; i++)
	{
		int count = 1;
		string tempVariable = variablesArray[i];
		variablesArray[i] = "";
		for (int j = i; j < totalCounter; j++)
		{
			if ((tempVariable.compare(variablesArray[j]) == 0) && (tempVariable.compare("") != 0))
			{
				variablesArray[j] = "";
				count++;
			}
		}
		if ((tempVariable.compare("") != 0) && (tempVariable.compare(" ") != 0))
		{
			cout << "   " << tempVariable << ": cпен ";
			printf_s("%d\n", count - 1);
			cout << endl;
		}
		count = 0;
	}
}

void SetTextColor(int textColor)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((Black << 4) | textColor));
}



