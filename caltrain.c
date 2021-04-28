#include "pintos_thread.h"

struct station {
	//khai bao cho ngoi con trong, khach dang cho, 
	int seats, waiting, boarding;

	struct lock lock;
	struct condition train_leave;
	struct condition seat_avalable;
};

void
station_init(struct station *station)
{
	station->seats = 0;
	station->waiting = 0;
	station->boarding = 0;

	lock_init(&station->lock);
	cond_init(&station->train_leave);
	cond_init(&station->seat_avalable);
}


void
station_load_train(struct station *station, int count)
{
	lock_acquire(&station->lock);
	station->seats = count; //Train comed
	cond_broadcast(&station->seat_avalable, &station->lock); //Send signal to station
	//watting for passengers
	while (station->seats >0 && station->waiting >0)
	{
		//Waiting for signal to leave
		cond_wait(&station->train_leave, &station->lock);
	}
	// When train leave seats = 0
	station->seats = 0;
	lock_release(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	station->waiting ++;
	// Waiting for train coming, 
	// when seats !=0, 
	// passengers can board the train
	while (station->seats == 0)
	{
		cond_wait(&station->seat_avalable, &station->lock);
	}
	station->boarding ++;
	station->seats --;
	station->waiting --;

	lock_release(&station->lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->boarding--; //when all passengers already in train, boarding = 0
	//Check waiting and boading, if both of them == 0 -> train can leave
	if ((station->seats == 0 || station->waiting == 0) && station->boarding == 0)
	{
		// Send signal to train for leaving
		cond_signal(&station->train_leave, &station->lock);
	}
	lock_release(&station->lock);
}
