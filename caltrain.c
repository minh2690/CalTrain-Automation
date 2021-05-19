#include "pintos_thread.h"

struct station {
	//khai bao cho ngoi, thoi gian cho tau, khach dang len tau
	int seats, waiting, boarding;

	struct lock lock; //Khai bao khoa bao ve bien global
	struct condition train_leave; //Khai bao bien dieu kien tau roi di
	struct condition seat_avalable; // Khai bao bien dieu kien cho ngoi san sang
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
	station->seats = count; 
	// printf("train come\n");
	//Gui tin hieu san sang cho hanh khach len tau
	cond_broadcast(&station->seat_avalable, &station->lock); 
	//Khi cho ngoi va thoi gian cho > 0 -> thread di ngu
	while (station->seats >0 && station->waiting >0)
	{
		cond_wait(&station->train_leave, &station->lock);
	}
	// When train leave seats = 0
	//printf("train leave %d\n", station->seats);
	station->seats = 0;
	lock_release(&station->lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	//Thoi gian cho tau den tang len 1
	
	station->waiting ++;

	while (station->seats == 0)
	{
		//Cho tin hieu tau den
		cond_wait(&station->seat_avalable, &station->lock);
	}
	//Khi tau da san sang, moi nguoi bat dau len tau -> boarding ++
	//Cho ngoi giam xuong -> seats --
	//Thoi gian cho tau = 0 -> waiting --
	station->boarding ++;
	station->seats --;
	station->waiting --;
	printf("wait for train %d\n", station->seats);
	printf("wait for train %d\n", station->waiting);
	lock_release(&station->lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->boarding--; //when all passengers already in train, boarding = 0
	//Check waiting and boading, if both of them == 0 -> train can leave
	// printf("on board1\n");
	if((station->seats == 0 || station->waiting == 0) && station->boarding == 0)
	{
		// printf("on board2\n");
		// Send signal to train for leaving
		cond_signal(&station->train_leave, &station->lock);
	}
	lock_release(&station->lock);
}
