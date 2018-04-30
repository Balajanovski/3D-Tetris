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

int RandomNumberComponent::rng(int lower_bound, int upper_bound) {
    return std::uniform_int_distribution<>(lower_bound, upper_bound)(gen);
}