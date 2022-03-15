#include <iostream>
#include <random>
#include <time.h>

int diff;

std::minstd_rand random_engine() {
	std::minstd_rand eng{time(0)};
	return eng;
}

int getRandomNumber() {
	return random_engine()() % 1000;
}

bool compare(std::string check, int number) {
	if (check == " ") check = "0";
	if (std::stoi(check) == number) return true;
	diff = std::stoi(check) - number;
	return false;
}

std::string returnDiff() {
	if (diff < 0) {
		return "The number is too small";
	}
	if (diff > 0) {
		return "The number is too big";
	}
}