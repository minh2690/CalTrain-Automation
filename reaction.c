#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	int h;// Khoi tao nguyen tu H

	struct lock lock; // Khoa lock
	struct condition h_full; // Bien dieu kien co du nguyen tu H
	struct condition wait_done; // Bien dieu kien giai phong 2 nguyen tu H
};

void
reaction_init(struct reaction *reaction)
{
	reaction->h = 0;
	lock_init(&reaction->lock);
	cond_init(&reaction->h_full);
	cond_init(&reaction->wait_done);
}

void
reaction_h(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	reaction->h ++;
	// Gui tin hieu thong bao H da du
	cond_signal(&reaction->h_full, &reaction->lock);
	// Cho phan ung xong de release
	cond_wait(&reaction->wait_done, &reaction->lock);
	lock_release(&reaction->lock);
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	// Kiem tra dieu kien neu nguyen tu H it hon 2 thi di ngu
	while(reaction->h < 2)
	{
		cond_wait(&reaction->h_full, &reaction->lock);
	}
	// Phuong thuc make_water
	make_water();
	// Khi phan ung xong giam H di 2
	reaction->h -= 2;

	// Gui 2 tin hieu de giai phong hai thread H
	cond_signal(&reaction->wait_done, &reaction->lock);
	cond_signal(&reaction->wait_done, &reaction->lock);
	lock_release(&reaction->lock);
}
