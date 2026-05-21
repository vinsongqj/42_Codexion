#include "codexion.h"

static int	get_higher_child(t_heap *heap, int i, int s)
{
	int	left;
	int	right;

	left = 2 * i + 1;
	right = 2 * i + 2;
	if (left < heap->size && is_higher(heap->nodes[left], heap->nodes[s]))
		s = left;
	if (right < heap->size && is_higher(heap->nodes[right], heap->nodes[s]))
		s = right;
	return (s);
}

void	heap_push(t_heap *heap, int coder_id, long long priority)
{
	int		i;
	t_node	tmp;

	pthread_mutex_lock(&heap->lock);
	i = heap->size++;
	heap->nodes[i].coder_id = coder_id;
	heap->nodes[i].priority = priority;
	while (i > 0 && is_higher(heap->nodes[i], heap->nodes[(i - 1) / 2]))
	{
		tmp = heap->nodes[i];
		heap->nodes[i] = heap->nodes[(i - 1) / 2];
		heap->nodes[(i - 1) / 2] = tmp;
		i = (i - 1) / 2;
	}
	pthread_mutex_unlock(&heap->lock);
}

void	heap_pop(t_heap *heap)
{
	int		i;
	int		s;
	t_node	tmp;

	pthread_mutex_lock(&heap->lock);
	if (heap->size == 0)
		return (pthread_mutex_unlock(&heap->lock), (void)0);
	heap->nodes[0] = heap->nodes[--heap->size];
	i = 0;
	while (1)
	{
		s = get_higher_child(heap, i, i);
		if (s == i)
			break ;
		tmp = heap->nodes[i];
		heap->nodes[i] = heap->nodes[s];
		heap->nodes[s] = tmp;
		i = s;
	}
	pthread_mutex_unlock(&heap->lock);
}

int	is_coder_next(t_heap *heap, int coder_id)
{
	int	res;

	res = 0;
	pthread_mutex_lock(&heap->lock);
	if (heap->size > 0 && heap->nodes->coder_id == coder_id)
		res = 1;
	pthread_mutex_unlock(&heap->lock);
	return (res);
}

void	heap_remove_coder(t_heap *heap, int coder_id)
{
	int		i;
	int		s;
	t_node	tmp;

	pthread_mutex_lock(&heap->lock);
	i = -1;
	while (++i < heap->size)
		if (heap->nodes[i].coder_id == coder_id)
			break ;
	if (i < heap->size)
	{
		heap->nodes[i] = heap->nodes[--heap->size];
		while (1)
		{
			s = get_higher_child(heap, i, i);
			if (s == i)
				break ;
			tmp = heap->nodes[i];
			heap->nodes[i] = heap->nodes[s];
			heap->nodes[s] = tmp;
			i = s;
		}
	}
	pthread_mutex_unlock(&heap->lock);
}