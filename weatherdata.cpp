#include "weatherdata.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <string.h>

using namespace std;


/* WeatherData class constructor */
WeatherData::WeatherData() {
    nil = new WeatherDataNode();
    root = nil;
    root->parent = nil;
    root->leftChild = nil;
    root->rightChild = nil;
}

/* WeatherData class destructor */
WeatherData::~WeatherData(){
	DeleteAll(root);
}

/* void DeleteAll(WeatherDataNode);
 * 	Description: Deletes all elements of the WeatherData class. Only called
 * 	by the destructor. Private method.
 * 
 * 	Pre-conditions: None
 * 	Post-Condition: The WeatherData class instance will be empty.
 */
void WeatherData::DeleteAll(WeatherDataNode * node) {
	if (node) {
		DeleteForecasts(node->fore);
		if (node->leftChild != nil)
			DeleteAll(node->leftChild);
		if (node->rightChild != nil)
			DeleteAll(node->rightChild);
    }
    delete node;
}

/* void DeleteForecasts(Forecast);
 * 	Description: Deletes all elements of a forecast data type. Private method.
 * 
 * 	Pre-condition: None
 * 	Post-condition: All forecast data encapsulated in a WeatherDataNode are
 *  	deleted.
 */
void WeatherData::DeleteForecasts(Forecast * node) {
	if (node->next) 
		DeleteForecasts(node->next);
	delete node;
}

/* bool foundCity(string name)
 * 	Description: Finds an instance of 'name' in the WeatherData class instance.
 * 		Returns true if found, false if not found. Case-sensitive. Public method.
 * 
 * 	Pre-condition: None
 * 	Post-condition: None
 */
bool WeatherData::foundCity(string name) {
	if (searchWeatherTree(root, name) != nil)
		return true;
	return false;
}

/* WeatherDataNode * searchWeatherTree(WeatherDataNode * node, string name);
 * 	Description: Searches the WeatherData class for an instance of 'name' starting at 
 * 		root 'node'. Private method. If found, returns the node instance. If not, returns nil.
 * 	
 * 	Pre-condition: None
 * 	Post-condition: None
 */
WeatherDataNode* WeatherData::searchWeatherTree(WeatherDataNode * node, string name) {
   if (node == nil)
       return nil;
   else if (node->loc.cityName == name)
       return node;
   else {
      if(name.compare(node->loc.cityName) < 0)
         return searchWeatherTree(node->leftChild,name);
      else
         return searchWeatherTree(node->rightChild,name);
   }
}

/* void addCity(string cityInfo);
 * 	Description: Adds a new city to the network based on the information given by 
 * 		'cityInfo'. Public method.
 * 
 * 	Pre-condition: cityInfo must be a string formatted from a json Yahoo! Weather API
 * 		query. 
 * 	Post-conditino: A new WeatherDataNode will be created and added to the tree database in the 
 * 		appropriate location.
 */
void WeatherData::addCity(string cityInfo) {
	WeatherDataNode * newCity = new WeatherDataNode();
	newCity->parent = nil;
	newCity->leftChild = nil;
	newCity->rightChild = nil;

	//Get Sun Data
	newCity->sunData.sunrise = cityInfo.substr(cityInfo.find("sunrise\":") + 11 , 7);
	newCity->sunData.sunset = cityInfo.substr(cityInfo.find("sunset\":") + 10 , 7);
	
	//Get Astronomy Data
	newCity->atmosData.humidity = cityInfo.substr(cityInfo.find("humidity") + 12, 2) + "%";
	
	cityInfo.erase(0, cityInfo.find("pressure\":"));
	newCity->atmosData.pressure = cityInfo.substr(cityInfo.find("pressure") + 12, cityInfo.find("\",") - (cityInfo.find("pressure") + 12)) + " psi";
	
	cityInfo.erase(0, cityInfo.find("rising\":"));
	newCity->atmosData.rising = cityInfo.substr(cityInfo.find("rising") + 10, cityInfo.find("\",") 
		- (cityInfo.find("rising") + 10));
	
	cityInfo.erase(0, cityInfo.find("visibility\":"));
	newCity->atmosData.visibility = cityInfo.substr(cityInfo.find("visibility") 
		+ 14, cityInfo.find("\" ") - (cityInfo.find("visibility") + 14)) + " mi";
	
	
	//Get curr temp and time
	cityInfo.erase(0, cityInfo.find("date\":"));
	newCity->timeOfQuery = cityInfo.substr(cityInfo.find("date") + 8, cityInfo.find("\",") 
		- (cityInfo.find("date") + 8));
		
	cityInfo.erase(0, cityInfo.find("temp\":"));
	newCity->currTemp = cityInfo.substr(cityInfo.find("temp") + 8, cityInfo.find("\",") 
		- (cityInfo.find("temp") + 8)) + " F";
		
	cityInfo.erase(0, cityInfo.find("text\":"));
	newCity->currWeather = cityInfo.substr(cityInfo.find("text") + 8, cityInfo.find("\" ") 
		- (cityInfo.find("text") + 8));
		
	
	//Get Forecast Data
	for (int i = 0; i < 5; i++) { 
		Forecast * temp = new Forecast();
		
		cityInfo.erase(0, cityInfo.find("date\":"));
		temp->date = cityInfo.substr(cityInfo.find("date") + 8, cityInfo.find("\",") 
			- (cityInfo.find("date") + 8));
			
		cityInfo.erase(0, cityInfo.find("day\":"));
		temp->day = cityInfo.substr(cityInfo.find("day") + 7, cityInfo.find("\",") 
			- (cityInfo.find("day") + 7));
			
		cityInfo.erase(0, cityInfo.find("high\":"));
		temp->high = cityInfo.substr(cityInfo.find("high") + 8, cityInfo.find("\",") 
			- (cityInfo.find("high") + 8)) + " F";
			
		cityInfo.erase(0, cityInfo.find("low\":"));
		temp->low = cityInfo.substr(cityInfo.find("low") + 7, cityInfo.find("\",") 
			- (cityInfo.find("low") + 7)) + " F";
			
		cityInfo.erase(0, cityInfo.find("text\":"));
		temp->storm = cityInfo.substr(cityInfo.find("text") + 8, cityInfo.find("\" ") 
			- (cityInfo.find("text") + 8));
		
		
		Forecast * x = newCity->fore;
		
		if (!x) {
			newCity->fore = temp;
		}
		else { 
			while(x->next) 
				x = x->next;
			x->next = temp;
		}
	}
	
	
	//Get Location Data
	cityInfo.erase(0, cityInfo.find("lat\":"));
	newCity->loc.lat = cityInfo.substr(cityInfo.find("lat") + 7, cityInfo.find("\",") - (cityInfo.find("lat") + 7));
	
	cityInfo.erase(0, cityInfo.find("long\":"));
	newCity->loc.lon = cityInfo.substr(cityInfo.find("long") + 8, cityInfo.find("\",") - (cityInfo.find("lon") + 8));	
	
	cityInfo.erase(0, cityInfo.find("city\":"));
	newCity->loc.cityName = cityInfo.substr(cityInfo.find("city\":") + 8, cityInfo.find("\",") - (cityInfo.find("city\":")+8));
	
	cityInfo.erase(0, cityInfo.find("country\":"));
	newCity->loc.country = cityInfo.substr(cityInfo.find("country\":") + 11, cityInfo.find("\",") - (cityInfo.find("country\":")+11));
	
	cityInfo.erase(0, cityInfo.find("region\":"));
	newCity->loc.region = cityInfo.substr(cityInfo.find("region\":") + 10, cityInfo.find("\" ") - (cityInfo.find("region\":")+10));

	
	//Get Wind Data
	cityInfo.erase(0, cityInfo.find("chill\":"));
	newCity->wind.chill = cityInfo.substr(cityInfo.find("chill") + 9, cityInfo.find("\",") - (cityInfo.find("chill") + 9)) + " F";
		
	cityInfo.erase(0, cityInfo.find("direction\":"));
	newCity->wind.direction = cityInfo.substr(cityInfo.find("direction") + 13, cityInfo.find("\",") - (cityInfo.find("direction") + 13));
	
	cityInfo.erase(0, cityInfo.find("speed\":"));
	newCity->wind.speed = cityInfo.substr(cityInfo.find("speed") + 9, cityInfo.find("\" ") - (cityInfo.find("speed") + 9)) + " mph";



	//Put weather data into tree by cityname.
	WeatherDataNode * x = root;
	
    if (root == nil) {
        root = newCity;
        root->isRed = false;
    }
    else {
        while (x != nil) {
            if (strcmp(newCity->loc.cityName.c_str(), x->loc.cityName.c_str()) < 0) {
                if (x->leftChild == nil) {
					x->leftChild = newCity;
					newCity->parent = x;
					break;
				}
				x = x->leftChild;
				continue;
			}
			if (strcmp(newCity->loc.cityName.c_str(), x->loc.cityName.c_str()) >= 0) {
				if (x->rightChild == nil) {
					x->rightChild = newCity;
					newCity->parent = x;
					break;
				}
				x = x->rightChild;
				continue;
			}
		}
	}
    rbAddFixup(newCity);
    return;
}

/* void rbAddFixup(WeatherDataNode * node);
 * 	Description: Private method called by addCity method. Ensures the 
 * 		balanced structure of the database.
 * 
 * 	Pre-condition: 'node' must be the new node added to the tree.
 * 	Post-conditino: The tree regains balance.
 */
void WeatherData::rbAddFixup(WeatherDataNode * node) {
   while (node != root && node->parent->isRed) {
      if (node->parent == node->parent->parent->leftChild) {
         WeatherDataNode *temp = node->parent->parent->rightChild;
         if (temp->isRed) {
            node->parent->isRed = false;
            temp->isRed = false;
            node->parent->parent->isRed = true;
            node = node->parent->parent;
         }
         else {
            if (node == node->parent->rightChild) {
               node = node->parent;
               leftRotate(node);
            }
            node->parent->isRed = false;
            node->parent->parent->isRed = true;
            rightRotate(node->parent->parent);
         }
      }
      else {
         WeatherDataNode *temp = node->parent->parent->leftChild;
         if (temp->isRed) {
            node->parent->isRed = false;
            temp->isRed = false;
            node->parent->parent->isRed = true;
            node = node->parent->parent;
         }
         else {
            if (node == node->parent->leftChild) {
               node = node->parent;
               rightRotate(node);
            }
            node->parent->isRed = false;
            node->parent->parent->isRed = true;
            leftRotate(node->parent->parent);
         }
      }
   }
   root->isRed = false;
}

/* void leftRotate(WeatherDataNode * node);
 * 	Description: Performs a left rotation about 'node'. Private method.
 * 
 * 	Pre-condition: 'node' is the appropriate node to performa a left rotatation
 * 		about.
 * 	Post-condition: The tree is rotated left about 'node'.
 */
void WeatherData::leftRotate(WeatherDataNode *node) {
    WeatherDataNode *x = node->rightChild;
    node->rightChild = x->leftChild;
    if (x->leftChild != nil)
        x->leftChild->parent = node;
    x->parent = node->parent;
    if (node->parent == nil)
        root = x;
    else if (node->parent->leftChild == node)
        node->parent->leftChild = x;
    else node->parent->rightChild = x;
    node->parent = x;
    x->leftChild = node;
}

/* void rightRotate(WeatherDataNode * node);
 * 	Description: Performs a right rotation about 'node'. Private method.
 * 
 * 	Pre-condition: 'node' is the appropriate node to performa a right rotatation
 * 		about.
 * 	Post-condition: The tree is rotated right about 'node'.
 */
void WeatherData::rightRotate(WeatherDataNode *node) {
    WeatherDataNode *x = node->leftChild;
    node->leftChild = x->rightChild;
    if (x->rightChild != nil)
        x->rightChild->parent = node;
    x->parent = node->parent;
    if (node->parent == nil)
        root = x;
    else if (node->parent->leftChild == node)
        node->parent->leftChild = x;
    else node->parent->rightChild = x;
    node->parent = x;
    x->rightChild = node;
}

/* void printCitiesByName();
 * 	Description: Public method to print all city names in alphabetical order.
 * 
 * 	Pre-condition: None.
 *	Post-condition: All city names will be printed to the command terminal window.
 */
void WeatherData::printCitiesByName() {
   printCitiesByName(root);
}

/* void printCitiesByName(WeatherDataNode * node);
 * 	Description: Private method called by printCitiesByName() that prints each city in the
 * 		database in alphabetical order.
 * 		
 *	Pre-condition: 'node' must be the root of the tree.
 * 	Post-condition: All city names will be printed to the command terminal window.
 */
void WeatherData::printCitiesByName(WeatherDataNode * node) {
   if(node->leftChild != nil)
       printCitiesByName(node->leftChild);

	cout << node->loc.cityName << endl;

   if(node->rightChild != nil)
       printCitiesByName(node->rightChild);
   return;
}

/* void getAstronomyData(string city);
 * 	Description: Prints sunrise and sunset times to the command window. Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 * 	Post-condition: Sunrise and sunset times are printed to the command window.
 */
void WeatherData::getAstronomyData(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << "The Sunrise for today is at " << x->sunData.sunrise << "." << endl;
	cout << "The sunset for today is at " << x->sunData.sunset << "." << endl;
}

/* void getAtmosphericData(string city);
 * 	Description: Prints atmospheric data about 'city' to the command window. 
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 * 	Post-condition: Astronomy data is printed to the command window.
 */
void WeatherData::getAtmosphericData(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << "The humidity right now is " << x->atmosData.humidity << "." << endl;
	cout << "The atmospheric pressure right now is " << x->atmosData.pressure << "." << endl;
	cout << "The visibility right now is " << x->atmosData.visibility << "." << endl;
}

/* void getForecast(string city);
 * 	Description: Prints 5-day forecast to command window for city name 'city'.
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 *  Post-condition: Printcs forecast to command window.
 */
void WeatherData::getForecast(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	Forecast * temp = x->fore;
	cout << "5 Day Forecast for " << x->loc.cityName << ", " << x->loc.region << ":" << endl;
	while (temp) {
		cout << "_____" + temp->day + ", " + temp->date + "_____" << endl;
		cout << "High: " << temp->high << endl;
		cout << "Low: " << temp->low << endl;
		cout << temp->storm << endl << endl;
		temp = temp->next;
	}
}

/* void getWindData(string city);
 * 	Description: Prints wind data to command window for city name 'city'.
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 *  Post-condition: Prints wind data to command window.
 */
void WeatherData::getWindData(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << "Chill Factor: " << x->wind.chill << endl;
	cout << "Direction: " << x->wind.direction << endl;
	cout << "Speed: " << x->wind.speed << endl;
}

/* void getLocation(string city);
 * 	Description: Prints location data to command window to city name 'city'.
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 * 	Post-conidition: Prints location data to command window.
 */
void WeatherData::getLocation(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << x->loc.cityName << ", " << x->loc.region << endl;
	cout << x->loc.country << endl;
	cout << "Latitude: " << x->loc.lat << endl;
	cout << "Longitude: " << x->loc.lon << endl;
}

/* void getCurrentWeather(string city);
 * 	Description: Prints current weather information to command window for city name 'city'.
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 * 	Post-condition: Prints weather information to command window.
 */
void WeatherData::getCurrentWeather(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << "As of " + x->timeOfQuery + ":" << endl;
	cout <<"Temperature: " << x->currTemp << endl;
	cout << x->currWeather << endl;
}

/* void getAllWeatherData(string city);
 * 	Description: Prints all weather data available to command window for city name 'city'.
 * 		Public method.
 * 
 * 	Pre-condition: City must be a city name currently in the database. The start of 
 * 		each word in the name must be capitalized and the rest lower-case.
 * 	Post-condition: Prints all current weather data to command line.
 */
void WeatherData::getAllWeatherData(string city) {
	WeatherDataNode * x = searchWeatherTree(root, city);
	cout << "Information for " << x->loc.cityName << ", " << x->loc.region << ":" << endl;
	getCurrentWeather(city);
	cout << "Location Information:" << endl;
	getLocation(city);
	getForecast(city);
	getAstronomyData(city);
	getAtmosphericData(city);
	getWindData(city);
	
}
/*
Prototype: void deleteCity(string)
Description: This is the function that gets called from the main menu.
             If the city entered has not been loaded the user is told so, otherwise this function calls the rbDelete function.
Example call: deleteCity(Istanbul)
Pre-conditions: The city entered must be a string
Post-conditions: If the city is found, it is deleted from the tree and the tree is then changed accordingly to preserve red-black properties
*/
void WeatherData::deleteCity(string city){
   WeatherDataNode *found = searchWeatherTree(root, city);

   if (found == nil)
      cout << "That city has is not loaded" << endl;
   else {
      rbDelete(found);
   }
}
/*
Prototype: void rbDeletey(WeatherDataNode *)
Description: This is the function that gets called by deleteCity
             It deletes the node entered and uses rbTransplant and rbDeleteFixup to preserve the red-black properties of the tree
Example call: rbDelete(deleteThisCity)
Pre-conditions: Must be passed a pointer to a city that is already in the tree
Post-conditions: Deletes the node from the tree and the tree is then changed accordingly to preserve red-black properties
*/
void WeatherData::rbDelete(WeatherDataNode * z){
   WeatherDataNode *y = z;
   bool origCol = y->isRed;//original color of the node
   WeatherDataNode *temp;
   if (z->leftChild == nil) {
      temp = z->rightChild;
      rbTransplant(z, z->rightChild);
   }
   else if (z->rightChild == nil) {
      temp = z->leftChild;
      rbTransplant(z, z->leftChild);
   }
   else {
      y = y->rightChild;
      while (y->leftChild != nil) {
         y = y->leftChild;
      }
      origCol = y->isRed;
      temp = y->rightChild;
      if (y->parent == z)
         temp->parent = z;
      else {
         rbTransplant(y, y->rightChild);
         y->rightChild = z->rightChild;
         y->rightChild->parent = y;
      }
      rbTransplant(z, y);
      y->leftChild = z->leftChild;
      y->leftChild->parent = y;
      y->isRed = z->isRed;
   }
   delete z;
   //if (!origCol)
     // rbDeleteFixup(temp);
}
/*
Prototype: void rbDeleteFixup(WeatherDataNode *)
Description: This function gets called if the tree has a red-black violation after the node is deleted
Example call: rbDeleteFixup(rightChild)
Pre-conditions: The tree has a red-black violation after a node is deleted
Post-conditions: The tree's red-black properties are restored
*/
void WeatherData::rbDeleteFixup(WeatherDataNode * x){
    while (x != root && !x->isRed) {
      if (x == x->parent->leftChild) {
         WeatherDataNode *w = x->parent->rightChild;
         if (w->isRed) {
            w->isRed = false;
            x->parent->isRed = true;
            leftRotate(x->parent);
            w = x->parent->rightChild;
         }
         if (!w->leftChild->isRed && !w->rightChild->isRed) {
            w->isRed = true;
            x = x->parent;
         }
         else {
            if (!w->rightChild->isRed) {
               w->leftChild->isRed = false;
               w->isRed = true;
               rightRotate(w);
               w = x->parent->rightChild;
            }
            w->isRed = x->parent->isRed;
            x->parent->isRed = false;
          //  w->isRed = x->parent->isRed;
            w->rightChild->isRed = false;
            leftRotate(x->parent);
            x = root;
         }
      }
      else  {
         WeatherDataNode *w = x->parent->leftChild;
         if (w->isRed) {
            w->isRed = false;
            x->parent->isRed = true;
            rightRotate(x->parent);
            w = x->parent->leftChild;
         }
         if (!w->rightChild->isRed && !w->leftChild->isRed) {
            w->isRed = true;
            x = x->parent;
         }
         else {
            if (!w->leftChild->isRed) {
               w->rightChild->isRed = false;
               w->isRed = true;
               leftRotate(w);
               w = x->parent->leftChild;
            }
            w->isRed = x->parent->isRed;
            x->parent->isRed = false;
            w->leftChild->isRed = false;
            rightRotate(x->parent);
            x = root;
         }
      }
  }
   x->isRed = false;
}
/*
Prototype: void rbTransplant(WeatherDataNode *,WeatherDataNode *)
Description: This function gets called if a parent node and it's child need to be swapped
Example call: rbTransplant(parentNode, parentNode->leftChild)
Pre-conditions: Parent node and one of it's child nodes need to be swapped to preserve the tree's binary and/or red-black properties
Post-conditions: The tree's binary and/or red-black properties are restored
*/
void WeatherData::rbTransplant(WeatherDataNode * u, WeatherDataNode * v){
    if(u->parent == nil){
        root = v;
    }

    else{
        if(u == u->parent->leftChild){
            u->parent->leftChild = v;
        }
        else{
            u->parent->rightChild = v;
            v->parent = u->parent;
        }
    }
}

//WeatherData
