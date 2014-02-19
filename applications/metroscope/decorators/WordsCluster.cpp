/*******************************************************************************
*   Copyright 2014 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

#include "WordsCluster.hpp"
#include <iostream>

const std::string decorators::WordsCluster::scDecoratorName("WordsCluster");
const  DecoratorManager::Registerer decorators::WordsCluster::mRegisterer(decorators::WordsCluster::scDecoratorName, &decorators::WordsCluster::create);

decorators::FiducialDecorator *decorators::WordsCluster::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tCardStrings = pSetting["possible_words"];
		int tNumCards = pSetting["num_possible_words"];
		WordsCard **tWordsCards = new WordsCard *[tNumCards];
		for (int i = 0; i < tNumCards; ++i) {
			tWordsCards[i] = (WordsCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
		}

		return new decorators::WordsCluster(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["position"]), tWordsCards, tNumCards);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::WordsCluster::WordsCluster(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTextPosition, WordsCard **pPieces, int pNumPieces):
FiducialDecorator(pDecoratorManager, pMarker),
mMessagePositionMarker(pTextPosition),
mNumPieces(pNumPieces),
mPieces(pPieces)
{
}

void decorators::WordsCluster::update() {
	if (mMarker->isPresent())
	{
		displayClusters();

		bool isCorrect = true;
		//TODO: implement the logic for getting whether the clusters are correctly made

		//TODO: implement the display of different messages for different cases
		if(isCorrect){
			displayMessage();
		}

	}

}


/*Just displays a text message in the designated position marker, with the hint or message for the students*/
void decorators::WordsCluster::displayMessage(){
	if(mMarker->isPresent()){

		getPresentPieces();

		if(mPresentPieces.size()>0){

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scCLUSTER_CORRECT_MSG.c_str(), scTEXT_DELIMITERS,
						mMessagePositionMarker->getCenter().x, mMessagePositionMarker->getCenter().y, scCLUSTER_MSG_WIDTH,
						false, scCLUSTER_MSG_SCALE,
						scGREEN.r, scGREEN.g, scGREEN.b, scGREEN.a);
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

/*Just displays a text message in the designated position marker, with the sentence*/
void decorators::WordsCluster::displayClusters(){
	if(mMarker->isPresent()){

		getClusters();

		for(int i=0;i<mClusters.size();i++){//we iterate over the clusters

			//we calculate the cluster's center and radius
			std::vector<wykobi::point2d<float>> clusterPoints;
			for(int ip=0; ip<mClusters[i].size();ip++) clusterPoints.push_back(mClusters[i][ip]->GetLocation());

			//Depending on the number of points, we create a polygon... or just a point or a line
			wykobi::point2d<float> center;
			if(mClusters[i].size()==1) center = mClusters[i][0]->GetLocation();
			else if(mClusters[i].size()==2){
				center.x = (mClusters[i][0]->GetLocation().x+mClusters[i][1]->GetLocation().x)/2;
				center.y = (mClusters[i][0]->GetLocation().y+mClusters[i][1]->GetLocation().y)/2;
			}else{
				wykobi::polygon<float,2> cardsPolygon = wykobi::make_polygon<float>(clusterPoints);
				center = wykobi::centroid(cardsPolygon);
			}

			float radius = 0;
			for(int ip=0; ip<mClusters[i].size();ip++){
				wykobi::point2d<float> position = mClusters[i][ip]->GetLocation();
				if(wykobi::distance(center.x, center.y, position.x, position.y)>radius) radius = wykobi::distance(center.x, center.y, position.x, position.y);
			}

			radius += scCLUSTER_OVERFLOW;

			//for each cluster, we draw a circle with radius equal to the distance to the furthest piece + some other distance
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderEllipse(center.x, center.y, radius, radius, scBLACK.r, scBLACK.g, scBLACK.b);
			mDecoratorManager.GetDisplay().PopTransformation();


		}


	}
}

void decorators::WordsCluster::getPresentPieces(){
	mPresentPieces.clear();

	for(int i=0; i<mNumPieces;i++){
		if(mPieces[i]->getMarker().isPresent()){
			mPresentPieces.push_back(mPieces[i]);
		}
	}

}

void decorators::WordsCluster::getClusters(){

	getPresentPieces();

	//TODO: Do a more "intelligent" clustering - for now, we just look at the distance between cards, and check that it is less than a certain value

	if(mPresentPieces.size()>0){

		mClusters.clear();

		for(int i=0; i<mPresentPieces.size(); i++) {

			bool addedToCluster = false;

			wykobi::point2d<float> presentPosition = mPresentPieces[i]->GetLocation();

			//We go over the clusters arrays and compare the distance from the current piece to each piece
			if(mClusters.size()>0){

				for(int ic = 0; ic<mClusters.size(); ic++){//iterate over the clusters

					if(mClusters[ic].size()>0){
						for(int ip=0; ip<mClusters[ic].size(); ip++){//iterate over the pieces in the cluster
							wykobi::point2d<float> piecePosition = mClusters[ic][ip]->GetLocation();
							if(wykobi::distance(presentPosition,piecePosition)<scCLUSTER_RANGE){//if the distance is less than the range for clustering, we add the piece to this cluster
								mClusters[ic].push_back(mPresentPieces[i]);
								addedToCluster = true;
								break;
							}

						}

					}
					//if, at this point we have already added the piece to a cluster, we go to the next piece
					if(addedToCluster) break;

				}
				//if the piece was not added to any cluster, we create a new cluster with the piece
				if(!addedToCluster){
					std::vector<WordsCard *> newCluster;
					newCluster.push_back(mPresentPieces[i]);
					mClusters.push_back(newCluster);
				}

			}else{
				//if we had no clusters, we create the first one
				std::vector<WordsCard *> newCluster;
				newCluster.push_back(mPresentPieces[i]);
				mClusters.push_back(newCluster);
			}



		}




	}


}

