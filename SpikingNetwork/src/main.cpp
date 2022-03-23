#include "SpikingNetwork.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#include <CL/cl.hpp>

//converts a tests passed int into a tests passed string for console output
std::string ConvertTestsPassedToString(int TestsPassed, int NumTests)
{
	std::string TestsPassedString = "\u001b[94m[";

	for(int i = 0; i < NumTests; i++)
	{
		TestsPassedString += (((TestsPassed >> i) & 0b1) == 0b1) ? "\u001b[32mP" : "\u001b[31mf";
	}

	return TestsPassedString + "\u001b[94m]\u001b[0m";
}

int main()
{
	//TODO(aria): this
	/*----------------------*/
	/*-----Neuron Tests-----*/
	/*----------------------*/
#pragma region Neuron Tests

#pragma endregion

	//TODO(aria): this
	/*--------------------------*/
	/*-----Connection Tests-----*/
	/*--------------------------*/
#pragma region ConnectionTests

#pragma endregion

	//TODO(aria): this
	/*--------------------*/
	/*-----Lobe Tests-----*/
	/*--------------------*/
#pragma region Lobe Tests

#pragma endregion

	/*---------------------------*/
	/*-----STDPHandler Tests-----*/
	/*---------------------------*/
#pragma region STDPHandler Tests

	std::cout << "\nBegining STDPHandler tests!" << std::endl;
	unsigned int STDPHandlerTestsPassed = 0;

	/*STDPHandler::STDPHandler()*/
	std::cout << "\u001b[32mBegin Test 1\u001b[0m" << std::endl;
	STDPHandler Handler;
	try
	{
		//create a new handler instance
		Handler = STDPHandler();
		
		STDPHandlerTestsPassed += 0b1;
		std::cout << "\u001b[32mTest 1 Passed\u001b[0m\n" << std::endl;
	}
	catch(std::exception &ex)
	{
		std::cout << "\u001b[33mTest 1 failed! STDPHandler::STDPHandler(): \u001b[31m" << ex.what() << "\u001b[0m\n" << std::endl;
	}

	/*STDPHandler::DoSTDP()*/
	std::cout << "\u001b[32mBegin Test 2\u001b[0m" << std::endl;
	if((STDPHandlerTestsPassed & 0b1) == 0b1)
	{
		try
		{
			//dummy data
			std::vector<unsigned long long> TargetTimes = { 0, 11, 50, 0, 5 };
			std::vector<unsigned long long> SourceTimes = { 5, 0, 25, 11, 0 };
			std::vector<float> Weights = { 2.0f, 0.5f, 3.0f, 3.5f, 1.75f };
			//expected results
			std::vector<float> ExpectedResults{ 2.01149631, 0.362500012, 3, 3.50767016, 1.72156882 };

			//run stdp
			std::vector<float> NewWeights = Handler.DoSTDP(SourceTimes, TargetTimes, Weights);
			//output the vector contents
			std::cout << "\u001b[36mOld Weight      New Weight      Expected Result \u001b[33m" << std::endl;
			for (int i = 0; i < 5; i++)
			{
				std::cout << std::left << std::setw(16) << std::setfill(' ') << Weights[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << NewWeights[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << ExpectedResults[i] << std::endl;
			}
			std::cout << "\u001b[0m";
			if (NewWeights == ExpectedResults)
			{
				std::cout << "\u001b[32mTest 2 Passed\u001b[0m\n" << std::endl;
				STDPHandlerTestsPassed += 0b10;
			}
			else
			{
				std::cout << "\u001b[31mTest 2 Failed: Incorrect results\u001b[0m\n" << std::endl;
			}
		}
		catch(std::exception &ex)
		{
			std::cout << "\u001b[33mTest 2 Failed! STDPHandler::DoSTDP(): \u001b[31m" << ex.what() << "\u001b[0m\n" << std::endl;
		}
	}
	else 
	{
		std::cout << "\u001b[33mUnable to do STDPHandler test 2! Cannot continue without test 1! \u001b[31mAuto-failing...\u001b[0m\n" << std::endl;
	}

	/*Multiple instances of STDPHandler*/
	std::cout << "\u001b[32mBegin Test 3\u001b[0m" << std::endl;
	STDPHandler Handler2, Handler3;
	try
	{
		//create another handler instance
		Handler2 = STDPHandler();
		Handler3 = STDPHandler();

		STDPHandlerTestsPassed += 0b100;
		std::cout << "\u001b[32mTest 3 Passed\u001b[0m\n" << std::endl;
	}
	catch (std::exception& ex)
	{
		std::cout << "\u001b[33mTest 3 failed! STDPHandler::STDPHandler(): \u001b[31m" << ex.what() << "\u001b[0m\n" << std::endl;
	}

	/*Multiple instances of STDPHandler with STDPHandler::DoSTDP*/
	std::cout << "\u001b[32mBegin Test 4\u001b[0m" << std::endl;
	if ((STDPHandlerTestsPassed & 0b100) == 0b100)
	{
		try
		{
			//dummy data
			std::vector<unsigned long long> TargetTimes = { 0, 11, 50, 0, 5 };
			std::vector<unsigned long long> SourceTimes = { 5, 0, 25, 11, 0 };
			std::vector<float> Weights = { 2.0f, 0.5f, 3.0f, 3.5f, 1.75f };
			//expected results
			std::vector<float> ExpectedResults{ 2.01149631, 0.362500012, 3, 3.50767016, 1.72156882 };

			//run stdp
			std::vector<float> NewWeights = Handler2.DoSTDP(SourceTimes, TargetTimes, Weights);
			std::vector<float> NewWeights2 = Handler2.DoSTDP(SourceTimes, TargetTimes, Weights);
			//output the vector contents
			std::cout << "\u001b[36mOld Weight      New Weight      Expected Result \u001b[33m" << std::endl;
			for (int i = 0; i < 5; i++)
			{
				std::cout << std::left << std::setw(16) << std::setfill(' ') << Weights[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << NewWeights[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << ExpectedResults[i] << std::endl;
			}
			std::cout << "\u001b[0m";
			std::cout << "\u001b[36mOld Weight      New Weight      Expected Result \u001b[33m" << std::endl;
			for (int i = 0; i < 5; i++)
			{
				std::cout << std::left << std::setw(16) << std::setfill(' ') << Weights[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << NewWeights2[i];
				std::cout << std::left << std::setw(16) << std::setfill(' ') << ExpectedResults[i] << std::endl;
			}
			std::cout << "\u001b[0m";
			if (NewWeights == ExpectedResults && NewWeights2 == ExpectedResults)
			{
				std::cout << "\u001b[32mTest 4 Passed\u001b[0m\n" << std::endl;
				STDPHandlerTestsPassed += 0b1000;
			}
			else
			{
				std::cout << "\u001b[31mTest 4 Failed: Incorrect results\u001b[0m\n" << std::endl;
			}
		}
		catch (std::exception& ex)
		{
			std::cout << "\u001b[33mTest 4 Failed! STDPHandler::DoSTDP(): \u001b[31m" << ex.what() << "\u001b[0m\n" << std::endl;
		}
	}
	else
	{
		std::cout << "\u001b[33mUnable to do STDPHandler test 4! Cannot continue without test 3! \u001b[31mAuto-failing...\u001b[0m\n" << std::endl;
	}
	std::cout << "\nFinal Result: " << ConvertTestsPassedToString(STDPHandlerTestsPassed, 4) << std::endl;
#pragma endregion

	//TODO(aria): this
	/*------------------------------*/
	/*-----SpikingNetwork Tests-----*/
	/*------------------------------*/
#pragma region SpikingNetwork Tests

#pragma endregion

}