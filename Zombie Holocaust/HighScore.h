#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

class HighScore
{
public:
	HighScore();
	~HighScore();
	void Initialise();
	void Reset();
	void UpdateHighscore(float s);
	std::vector<float> GetHighscore();

	int score;

	std::vector<float>  _hg;

};

