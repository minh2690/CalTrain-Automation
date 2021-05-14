#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	int h;

	struct lock lock;
	struct condition h_full;
	struct condition wait_done;
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
	cond_signal(&reaction->h_full, &reaction->lock);
	cond_wait(&reaction->wait_done, &reaction->lock);
	lock_release(&reaction->lock);
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	while (reaction->h < 2)
	{
		cond_wait(&reaction->h_full, &reaction->lock);
	}
	reaction->h -= 2;
	make_water();
	cond_signal(&reaction->wait_done, &reaction->lock);
	cond_signal(&reaction->wait_done, &reaction->lock);
	lock_release(&reaction->lock);
}
