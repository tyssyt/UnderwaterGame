// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Resources/Resource.h"

#include <vector>


struct XD_API Ingredient {
    Ingredient(int amount, const UResource* resource);
    ~Ingredient();

    const int amount;
    const UResource* const resource;
};

typedef Ingredient Result;

struct XD_API Recipe {
    Recipe(int time, std::initializer_list<Ingredient> ingredients, std::initializer_list<Result> results);
    Recipe(int time, std::initializer_list<Ingredient> ingredients, Result result);
    ~Recipe();

    const int time;
    const std::vector<Ingredient> ingredients;
    const std::vector<Result> results;

};
