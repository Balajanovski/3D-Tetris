// 
// Created by Balajanovski on 28/04/2018.
//

#include "RandomNumberComponent.h"

RandomNumberComponent::RandomNumberComponent() : gen(std::random_device()()) { }

void RandomNumberComponent::seed() {
    gen.seed(std::random_device()());
}

void RandomNumberComponent::seed(int s) {
    gen.seed(s);
}

int RandomNumberComponent::rng(int upper_bound, int lower_bound) {
    return std::uniform_int_distribution<std::mt19937::result_type>(lower_bound, upper_bound)(gen);
}