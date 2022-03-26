#include "SpikingNetwork.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

int main()
{
	//create a new handler instance
	std::cout << "Begin Buffer Test!" << std::endl;
	STDPHandler Handler = STDPHandler();

	//dummy data
	std::vector<unsigned long long> TargetTimes = { 0, 11, 50, 0, 5 };
	std::vector<unsigned long long> SourceTimes = { 5, 0, 25, 11, 0 };
	std::vector<float> Weights = { 2.0f, 0.5f, 3.0f, 3.5f, 1.75f };

	//run stdp
	std::vector<float> NewWeights = Handler.DoSTDP(SourceTimes, TargetTimes, Weights);
	//output the vector contents
	for (int i = 0; i < 5; i++)
	{
		std::cout << "Old Weight: " << Weights[i] << " \n\tNew Weight: " << NewWeights[i] << "\n" << std::endl;
	}
	std::cout << "\nTest Finished!\n" << std::endl;

	int ErrCode = 0;

	//create a spiking network
	SpikingNetwork ExampleNet = SpikingNetwork("Genomes/test_genome", 2, 2, &ErrCode, true);

	//output some information
	std::cout << "Neuron Info: " << ExampleNet.InputCount() << ", " << ExampleNet.NeuronCount() << ", " << ExampleNet.OutputCount() << std::endl;
	std::cout << "Lobe Info: " << ExampleNet.LobeCount() << std::endl;

	//feed in some dummy data
	ExampleNet.SetInputs({5, 3}, &ErrCode);
	//run the network and output the results for 100 iterations
	for(int i = 0; i < 10; i++)
	{
		ExampleNet.PerformUpdate(&ErrCode);
		std::vector<float> Results = ExampleNet.GetOutputs(&ErrCode);
		std::cout << "Iteration " << i << ": " << Results[0] << ", " << Results[1] << std::endl;
	}

	return 0;
}


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

//TODO(aria): implement literally all of these
//neuron tests
bool Neuron_SetParams();
bool Neuron_GetParams();

bool Neuron_SetConnectionParams();
bool Neuron_GetConnectionParams_();

bool Neuron_CalculateNewValue();

bool Neuron_SetState();
bool Neuron_SetValue();
bool Neuron_GetValue();
bool Neuron_GetSpikeState();

bool Neuron_WriteStateToFile();
bool Neuron_LoadStateFromBytes();

bool Neuron_GetConnectionCount();

bool Neuron_Neuron();

bool Neuron_AddConnectio();
bool Neuron_RemoveConnection();

//connection tests
bool Connection_SetParams();
bool Connection_GetParams();

bool Connection_Connection();
bool Connection_Connection();

bool Connection_CreateConnection();

bool Connection_TryAddValue();

bool Connection_WriteConnectionToFile();

//lobe tests
bool Lobe_AddNeuron();
bool Lobe_RemoveNeuron();
bool Lobe_GetNeuronCount();

bool Lobe_WriteLobeToChromosome();

bool Lobe_DoSTDP();

//STDPHandler tests
bool STDPHandler_STDPHandler();
bool STDPHandler_DoSTDP();

bool STDPHandler_M_STDPHandler();
bool STDPHandler_M_DoSTDP();

//spikingnetwork tests
bool SpikingNetwork_SpikingNetwork();
bool SpikingNetwork_D_SpikingNetwork();

bool SpikingNetwork_LoadMentalState();
bool SpikingNetwork_StoreMentalState();

bool SpikingNetwork_SaveNetwork();

bool SpikingNetwork_InputCount();
bool SpikingNetwork_OutputCount();
bool SpikingNetwork_NeuronCount();
bool SpikingNetwork_LobeCount();

bool SpikingNetwork_AddLobe();
bool SpikingNetwork_RemoveLobe();
bool SpikingNetwork_GetLobeNeuronCount();

bool SpikingNetwork_AddNeuronToLobe();

bool SpikingNetwork_AddNeuron();
bool SpikingNetwork_RemoveNeuron();
bool SpikingNetwork_AddConnection();
bool SpikingNetwork_RemoveConnection();
bool SpikingNetwork_GetNeuronConnectionCount();

bool SpikingNetwork_SetInputs();
bool SpikingNetwork_GetOutputs();
bool SpikingNetwork_PerformUpdate();

bool SpikingNetwork_GetNeuronPtr();
bool SpikingNetwork_GetNeuronID();
bool SpikingNetwork_GetNeuronIndex();

bool SpikingNetwork_SetNeuronParams();
bool SpikingNetwork_GetNeuronParams();

bool SpikingNetwork_SetConnectionParams();
bool SpikingNetwork_GetConnectionParams();

//TODO(aria): this
void DoUnitTests()
{
	//TODO(aria): this
	/*----------------------*/
	/*-----Neuron Tests-----*/
	/*----------------------*/
#pragma region Neuron Tests

	std::cout << "Begining Neuron tests!" << std::endl;
	unsigned int NeuronTestsPassed = 0;

	/*Neuron::Neuron(unsigned char*)*/
	std::cout << "\u001b[32mBegin Test 1\u001b[0m" << std::endl;

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