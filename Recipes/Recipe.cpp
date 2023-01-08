// Fill out your copyright notice in the Description page of Project Settings.

#include "Recipe.h"


Ingredient::Ingredient(int amount, const UResource* resource) : amount(amount), resource(resource) {}
Ingredient::~Ingredient() {}

Recipe::Recipe(int time, std::initializer_list<Ingredient> ingredients, Result result) : time(time), ingredients(ingredients), results(std::vector<Ingredient>{result}) {}
Recipe::Recipe(int time, std::initializer_list<Ingredient> ingredients, std::initializer_list<Result> results) : time(time), ingredients(ingredients), results(results) {}
Recipe::~Recipe() {}
