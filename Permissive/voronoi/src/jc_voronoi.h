/*

ABOUT:

	A fast single file 2D voronoi diagram generator.

LICENSE:

	The MIT License (MIT)

	Copyright (c) 2015 Mathias Westerdahl

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.


DISCLAIMER:

	This software is supplied "AS IS" without any warranties and support

USAGE:

	The api is very small:

	void jcv_diagram_generate( int num_points, const jcv_point* points, int img_width, int img_height, jcv_diagram* diagram );
	void jcv_diagram_generate_useralloc( int num_points, const jcv_point* points, int img_width, int img_height, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* diagram );
	void jcv_diagram_free( jcv_diagram* diagram );

	const jcv_site* jcv_diagram_get_sites( const jcv_diagram* diagram );
	const jcv_edge* jcv_diagram_get_edges( const jcv_diagram* diagram );


	An example usage:

	#define JC_VORONOI_IMPLEMENTATION
	// If you wish to use doubles
	//#define JCV_REAL_TYPE double
	//#define JCV_FABS fabs
	//#define JCV_ATAN2 atan2
	#include "jc_voronoi.h"

	void draw_edges(const jcv_diagram* diagram);
	void draw_cells(const jcv_diagram* diagram);

	void generate_and_draw(int numpoints, const jcv_point* points, int imagewidth, int imageheight)
	{
		jcv_diagram diagram;
		memset(&diagram, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(count, points, imagewidth, imageheight, &diagram );

		draw_edges(diagram);
		draw_cells(diagram);

		jcv_diagram_free( &diagram );
	}

	void draw_edges(const jcv_diagram* diagram)
	{
		// If all you need are the edges
		const jcv_edge* edge = jcv_diagram_get_edges( diagram );
		while( edge )
		{
			draw_line(edge->pos[0], edge->pos[1]);
			edge = edge->next;
		}
	}

	void draw_cells(const jcv_diagram* diagram)
	{
		// If you want to draw triangles, or relax the diagram,
		// you can iterate over the sites and get all edges easily
		const jcv_site* sites = jcv_diagram_get_sites( diagram );
		for( int i = 0; i < diagram->numsites; ++i )
		{
			const jcv_site* site = &sites[i];

			const jcv_graphedge* e = site->edges;
			while( e )
			{
				draw_triangle( site->p, e->pos[0], e->pos[1]);
				e = e->next;
			}
		}
	}

	// Here is a simple example of how to do the relaxations of the cells
	void relax_points(const jcv_diagram* diagram, jcv_point* points)
	{
		const jcv_site* sites = jcv_diagram_get_sites(diagram);
		for( int i = 0; i < diagram->numsites; ++i )
		{
			const jcv_site* site = &sites[i];
			jcv_point sum = site->p;
			int count = 1;

			const jcv_graphedge* edge = site->edges;

			while( edge )
			{
				sum.x += edge->pos[0].x;
				sum.y += edge->pos[0].y;
				++count;
				edge = edge->next;
			}

			points[site->index].x = sum.x / count;
			points[site->index].y = sum.y / count;
		}
	}

 */

#ifndef JC_VORONOI_H
#define JC_VORONOI_H

#include <math.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JCV_REAL_TYPE
	#define JCV_REAL_TYPE float
#endif

#ifndef JCV_ATAN2
	#define JCV_ATAN2(_Y_, _X_)	atan2f(_Y_, _X_)
#endif

#ifndef JCV_FABS
	#define JCV_FABS(_X_)		fabsf(_X_)
#endif

typedef JCV_REAL_TYPE jcv_real;

#pragma pack(push, 1)

typedef struct _jcv_point
{
	jcv_real x;
	jcv_real y;
} jcv_point;

typedef struct _jcv_graphedge
{
	struct _jcv_graphedge*	next;
	struct _jcv_edge*		edge;
	struct _jcv_site*		neighbor;
	jcv_point				pos[2];
	jcv_real				angle;
} jcv_graphedge;

typedef struct _jcv_site
{
	jcv_point		p;
	int				index;	// Index into the original list of points
	jcv_graphedge* 	edges;	// The half edges owned by the cell
} jcv_site;

typedef struct _jcv_edge
{
	struct _jcv_edge*	next;
	jcv_site* 			sites[2];
	jcv_point			pos[2];
	jcv_real			a;
	jcv_real			b;
	jcv_real			c;
} jcv_edge;

typedef struct _jcv_diagram
{
	struct _jcv_context_internal* internal;
	jcv_edge*	edges;
	jcv_site*	sites;
	int			numsites;
	jcv_real 	width;
	jcv_real 	height;
} jcv_diagram;

#pragma pack(pop)


/** 
 * Uses malloc
 */
extern void jcv_diagram_generate( int num_points, const jcv_point* points, int img_width, int img_height, jcv_diagram* diagram );

typedef void* (*FJCVAllocFn)(void* userctx, size_t size);
typedef void (*FJCVFreeFn)(void* userctx, void* p);

/** Same as above, but allows the client to use a custom allocator
 */
extern void jcv_diagram_generate_useralloc( int num_points, const jcv_point* points, int img_width, int img_height, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* diagram );


/** Uses free (or the registered custom free function)
 */
extern void jcv_diagram_free( jcv_diagram* diagram );

/** Returns an array of sites, where each index is the same as the original input point array.
 */
extern const jcv_site* jcv_diagram_get_sites( const jcv_diagram* diagram );

/** Returns a linked list of all the voronoi edges
 */
extern const jcv_edge* jcv_diagram_get_edges( const jcv_diagram* diagram );


#ifdef __cplusplus
}
#endif

#endif // JC_VORONOI_H

#ifdef JC_VORONOI_IMPLEMENTATION

#include <math.h>	// sqrt

// INTERNAL FUNCTIONS

#if defined(_MSC_VER) && !defined(__cplusplus)
	#define inline __inline
#endif

// jcv_point

static inline int jcv_point_cmp(const void *p1, const void *p2)
{
	const jcv_point* s1 = (const jcv_point*) p1;
	const jcv_point* s2 = (const jcv_point*) p2;
	return (s1->y != s2->y) ? (s1->y < s2->y ? -1 : 1) : (s1->x < s2->x ? -1 : 1);
}

static inline int jcv_point_less( const jcv_point* pt1, const jcv_point* pt2 )
{
	return (pt1->y < pt2->y) || ((pt1->y == pt2->y) && (pt1->x < pt2->x));
}

static inline int jcv_point_eq( const jcv_point* pt1, const jcv_point* pt2 )
{
	return (pt1->y == pt2->y) && (pt1->x == pt2->x);
}

static inline int jcv_point_on_edge( const jcv_point* pt, jcv_real max_x, jcv_real max_y )
{
	return pt->x == (jcv_real)0 || pt->y == (jcv_real)0 || pt->x == max_x || pt->y == max_y;
}

static inline jcv_real jcv_point_dist_sq( const jcv_point* pt1, const jcv_point* pt2)
{
    jcv_real diffx = pt1->x - pt2->x;
    jcv_real diffy = pt1->y - pt2->y;
    return diffx * diffx + diffy * diffy;
}

static inline jcv_real jcv_point_dist( const jcv_point* pt1, const jcv_point* pt2 )
{
	jcv_real dx = pt1->x - pt2->x;
	jcv_real dy = pt1->y - pt2->y;
	return (jcv_real) sqrt(dx*dx + dy*dy);
}

// Structs

#pragma pack(push, 1)

typedef struct _jcv_halfedge
{
	jcv_edge*				edge;
	struct _jcv_halfedge*	left;
	struct _jcv_halfedge*	right;
	jcv_point				vertex;
	jcv_real				y;
	int 					direction; // 0=left, 1=right
	int						pqpos;
} jcv_halfedge;

typedef struct _jcv_memoryblock
{
	size_t sizefree;
	struct _jcv_memoryblock* next;
	char*  memory;
} jcv_memoryblock;


typedef int  (*FJCVPriorityQueueCompare)(const void* node1, const void* node2);
typedef void (*FJCVPriorityQueueSetpos)(const void* node, int pos);
typedef int  (*FJCVPriorityQueueGetpos)(const void* node);
typedef int  (*FJCVPriorityQueuePrint)(const void* node, int pos);

typedef struct _jcv_priorityqueue
{
	// Implements a binary heap
	FJCVPriorityQueueCompare	compare;		// implement the < operator
	FJCVPriorityQueueSetpos		setpos;
	FJCVPriorityQueueGetpos		getpos;
	int							maxnumitems;
	int							numitems;
	void**						items;
} jcv_priorityqueue;


typedef struct _jcv_context_internal
{
	void*				mem;
	jcv_edge*			edges;
	jcv_halfedge*		beachline_start;
	jcv_halfedge*		beachline_end;
	jcv_halfedge*		last_inserted;
	jcv_priorityqueue* 	eventqueue;

	jcv_site*			sites;
	jcv_site*			bottomsite;
	int					numsites;
	int					numsites_sqrt;
	int					currentsite;
	int					_padding;

	jcv_memoryblock*	memblocks;
	jcv_edge*			edgepool;
	jcv_halfedge*		halfedgepool;
	void**				eventmem;

	void*				memctx;	// Given by the user
	FJCVAllocFn			alloc;
	FJCVFreeFn			free;

	jcv_real 			width;
	jcv_real 			height;
} jcv_context_internal;

#pragma pack(pop)


static const int JCV_DIRECTION_LEFT  = 0;
static const int JCV_DIRECTION_RIGHT = 1;
static const jcv_real JCV_INVALID_VALUE = (jcv_real)-1;


void jcv_diagram_free( jcv_diagram* d )
{
	jcv_context_internal* internal = d->internal;
	void* memctx = internal->memctx;
	FJCVFreeFn freefn = internal->free;
	while(internal->memblocks)
	{
		jcv_memoryblock* p = internal->memblocks;
		internal->memblocks = internal->memblocks->next;
		freefn( memctx, p );
	}

	freefn( memctx, internal->mem );
}

const jcv_site* jcv_diagram_get_sites( const jcv_diagram* diagram )
{
	return diagram->internal->sites;
}

const jcv_edge* jcv_diagram_get_edges( const jcv_diagram* diagram )
{
	return diagram->internal->edges;
}

static void* jcv_alloc(jcv_context_internal* internal, size_t size)
{
	if( !internal->memblocks || internal->memblocks->sizefree < size )
	{
		size_t blocksize = 16 * 1024;
		if( size + sizeof(jcv_memoryblock) > blocksize )
			blocksize = size + sizeof(jcv_memoryblock);

		jcv_memoryblock* block = (jcv_memoryblock*)internal->alloc( internal->memctx, blocksize );
		size_t offset = sizeof(jcv_memoryblock);
		block->sizefree = blocksize - offset;
		block->next = internal->memblocks;
		block->memory = ((char*)block) + offset;
		internal->memblocks = block;
	}
	void* p = internal->memblocks->memory;
	internal->memblocks->memory += size;
	internal->memblocks->sizefree -= size;
	return p;
}

static jcv_edge* jcv_alloc_edge(jcv_context_internal* internal)
{
	if( internal->edgepool )
	{
		jcv_edge* edge = internal->edgepool;
		internal->edgepool = internal->edgepool->next;
		//memset(edge, 0, sizeof(struct jcv_edge));
		//edge = new(edge) jcv_edge;
		return edge;
	}

	jcv_edge* edge = (jcv_edge*)jcv_alloc(internal, sizeof(jcv_edge));
	//memset(edge, 0, sizeof(struct jcv_edge));
	//edge = new(edge) jcv_edge;
	return edge;
}

static jcv_halfedge* jcv_alloc_halfedge(jcv_context_internal* internal)
{
	if( internal->halfedgepool )
	{
		jcv_halfedge* edge = internal->halfedgepool;
		internal->halfedgepool = internal->halfedgepool->right;
		return edge;
	}

	jcv_halfedge* edge = (jcv_halfedge*)jcv_alloc(internal, sizeof(jcv_halfedge));
	memset(edge, 0, sizeof(jcv_halfedge));
	//edge = new(edge) jcv_halfedge;
	return edge;
}

static jcv_graphedge* jcv_alloc_graphedge(jcv_context_internal* internal)
{
	return (jcv_graphedge*)jcv_alloc(internal, sizeof(jcv_graphedge));
}

static void* jcv_alloc_fn(void* memctx, size_t size)
{
	(void)memctx;
	return malloc(size);
}

static void jcv_free_fn(void* memctx, void* p)
{
	(void)memctx;
	free(p);
}

// jcv_edge

static void jcv_edge_create(jcv_edge* e, jcv_site* s1, jcv_site* s2)
{
	e->next = 0;
	e->sites[0] = s1;
	e->sites[1] = s2;
	e->pos[0].x = JCV_INVALID_VALUE;
	e->pos[1].x = JCV_INVALID_VALUE;

	// Create line equation between S1 and S2:
	// jcv_real a = -1 * (s2->p.y - s1->p.y);
	// jcv_real b = s2->p.x - s1->p.x;
	// //jcv_real c = -1 * (s2->p.x - s1->p.x) * s1->p.y + (s2->p.y - s1->p.y) * s1->p.x;
	//
	// // create perpendicular line
	// jcv_real pa = b;
	// jcv_real pb = -a;
	// //jcv_real pc = pa * s1->p.x + pb * s1->p.y;
	//
	// // Move to the mid point
	// jcv_real mx = s1->p.x + dx * jcv_real(0.5);
	// jcv_real my = s1->p.y + dy * jcv_real(0.5);
	// jcv_real pc = ( pa * mx + pb * my );

	jcv_real dx = s2->p.x - s1->p.x;
	jcv_real dy = s2->p.y - s1->p.y;

	// Simplify it, using dx and dy
	e->c = dx * (s1->p.x + dx * (jcv_real)0.5) + dy * (s1->p.y + dy * (jcv_real)0.5);

	if( JCV_FABS(dx) > JCV_FABS(dy) )
	{
		e->a = (jcv_real)1;
		e->b = dy / dx;
		e->c /= dx;
	}
	else
	{
		e->a = dx / dy;
		e->b = (jcv_real)1;
		e->c /= dy;
	}
}

static int jcv_edge_clipline(jcv_edge* e, jcv_real width, jcv_real height)
{
	jcv_real pxmin = 0;
	jcv_real pxmax = width;
	jcv_real pymin = 0;
	jcv_real pymax = height;

	jcv_real x1, y1, x2, y2;
	jcv_point* s1;
	jcv_point* s2;
	if (e->a == (jcv_real)1 && e->b >= (jcv_real)0)
	{
		s1 = e->pos[1].x != JCV_INVALID_VALUE ? &e->pos[1] : 0;
		s2 = e->pos[0].x != JCV_INVALID_VALUE ? &e->pos[0] : 0;
	}
	else
	{
		s1 = e->pos[0].x != JCV_INVALID_VALUE ? &e->pos[0] : 0;
		s2 = e->pos[1].x != JCV_INVALID_VALUE ? &e->pos[1] : 0;
	};

	if (e->a == (jcv_real)1)
	{
		y1 = pymin;
		if (s1 != 0 && s1->y > pymin)
		{
			y1 = s1->y;
		}
		if( y1 > pymax )
		{
			y1 = pymax;
		}
		x1 = e->c - e->b * y1;
		y2 = pymax;
		if (s2 != 0 && s2->y < pymax)
			y2 = s2->y;

		if( y2 < pymin )
		{
			y2 = pymin;
		}
		x2 = (e->c) - (e->b) * y2;
		if( ((x1 > pxmax) & (x2 > pxmax)) | ((x1 < pxmin) & (x2 < pxmin)) )
		{
			return 0;
		}
		if (x1 > pxmax)
		{
			x1 = pxmax;
			y1 = (e->c - x1) / e->b;
		}
		else if (x1 < pxmin)
		{
			x1 = pxmin;
			y1 = (e->c - x1) / e->b;
		}
		if (x2 > pxmax)
		{
			x2 = pxmax;
			y2 = (e->c - x2) / e->b;
		}
		else if (x2 < pxmin)
		{
			x2 = pxmin;
			y2 = (e->c - x2) / e->b;
		}
	}
	else
	{
		x1 = pxmin;
		if( s1 != 0 && s1->x > pxmin )
			x1 = s1->x;
		if( x1 > pxmax )
		{
			x1 = pxmax;
		}
		y1 = e->c - e->a * x1;
		x2 = pxmax;
		if( s2 != 0 && s2->x < pxmax )
			x2 = s2->x;
		if( x2 < pxmin )
		{
			x2 = pxmin;
		}
		y2 = e->c - e->a * x2;
		if( ((y1 > pymax) & (y2 > pymax)) | ((y1 < pymin) & (y2 < pymin)) )
		{
			return 0;
		}
		if( y1 > pymax )
		{
			y1 = pymax;
			x1 = (e->c - y1) / e->a;
		}
		else if( y1 < pymin )
		{
			y1 = pymin;
			x1 = (e->c - y1) / e->a;
		}
		if( y2 > pymax )
		{
			y2 = pymax;
			x2 = (e->c - y2) / e->a;
		}
		else if( y2 < pymin )
		{
			y2 = pymin;
			x2 = (e->c - y2) / e->a;
		};
	};

	e->pos[0].x = x1;
	e->pos[0].y = y1;
	e->pos[1].x = x2;
	e->pos[1].y = y2;

	return 1;
}

static jcv_edge* jcv_edge_new(jcv_context_internal* internal, jcv_site* s1, jcv_site* s2)
{
	jcv_edge* e = jcv_alloc_edge(internal);
	jcv_edge_create(e, s1, s2);
	return e;
}


// jcv_halfedge

static void jcv_halfedge_link(jcv_halfedge* edge, jcv_halfedge* newedge)
{
	newedge->left = edge;
	newedge->right = edge->right;
	edge->right->left = newedge;
	edge->right = newedge;
}

static void jcv_halfedge_unlink(jcv_halfedge* he)
{
	he->left->right = he->right;
	he->right->left = he->left;
	he->left  = 0;
	he->right = 0;
}

static void jcv_halfedge_create(jcv_halfedge* he, jcv_edge* e, int dir)
{
	he->edge 		= e;
	he->left 		= 0;
	he->right		= 0;
	he->direction	= dir;
	he->pqpos		= 0;
	he->y			= 0;
}

static jcv_halfedge* jcv_halfedge_new(jcv_context_internal* internal, jcv_edge* e, int direction)
{
	jcv_halfedge* he = jcv_alloc_halfedge(internal);
	jcv_halfedge_create(he, e, direction);
	return he;
}

static void jcv_halfedge_delete(jcv_context_internal* internal, jcv_halfedge* he)
{
	he->right = internal->halfedgepool;
    internal->halfedgepool = he;
}

static jcv_site* jcv_halfedge_leftsite(const jcv_halfedge* he)
{
	return he->edge->sites[he->direction];
}

static jcv_site* jcv_halfedge_rightsite(const jcv_halfedge* he)
{
	return he->edge ? he->edge->sites[1 - he->direction] : 0;
}

static int jcv_halfedge_rightof(const jcv_halfedge* he, const jcv_point* p)
{
	const jcv_edge*	e = he->edge;
	const jcv_site*	topsite = e->sites[1];

	int right_of_site = p->x > topsite->p.x ? 1 : 0;
	if (right_of_site && he->direction == JCV_DIRECTION_LEFT)
		return 1;
	if (!right_of_site && he->direction == JCV_DIRECTION_RIGHT)
		return 0;

	jcv_real dxp, dyp, dxs, t1, t2, t3, yl;

	int above;
	if (e->a == (jcv_real)1)
	{
		dyp = p->y - topsite->p.y;
		dxp = p->x - topsite->p.x;
		int fast = 0;
		if( (!right_of_site & (e->b < (jcv_real)0)) | (right_of_site & (e->b >= (jcv_real)0)) )
		{
			above = dyp >= e->b * dxp;
			fast = above;
		}
		else
		{
			above = p->x + p->y * e->b > e->c;
			if (e->b < (jcv_real)0)
				above = !above;
			if (!above)
				fast = 1;
		};
		if (!fast)
		{
			dxs = topsite->p.x - e->sites[0]->p.x;
			above = e->b * (dxp * dxp - dyp * dyp)
					< dxs * dyp * ((jcv_real)1 + (jcv_real)2 * dxp / dxs + e->b * e->b);
			if (e->b < (jcv_real)0)
				above = !above;
		};
	}
	else // e->b == 1
	{
		yl = e->c - e->a * p->x;
		t1 = p->y - yl;
		t2 = p->x - topsite->p.x;
		t3 = yl - topsite->p.y;
		above = t1 * t1 > t2 * t2 + t3 * t3;
	};
	return (he->direction == JCV_DIRECTION_LEFT ? above : !above);
}


static inline int jcv_halfedge_compare( const jcv_halfedge* he1, const jcv_halfedge* he2 )
{
	return (he1->y > he2->y) || ((he1->y == he2->y) && (he1->vertex.x > he2->vertex.x));
}

static inline void jcv_halfedge_setpos( jcv_halfedge* he, int pos )
{
	he->pqpos = pos;
}

static inline int jcv_halfedge_getpos( const jcv_halfedge* he )
{
	return he->pqpos;
}

static int jcv_halfedge_intersect(const jcv_halfedge* he1, const jcv_halfedge* he2, jcv_point* out)
{
	const jcv_edge* e1 = he1->edge;
	const jcv_edge* e2 = he2->edge;

	jcv_real dx = e2->sites[1]->p.x - e1->sites[1]->p.x;
	jcv_real dy = e2->sites[1]->p.y - e1->sites[1]->p.y;

	if( dx == 0 && dy == 0 )
	{
		return 0;
	}

	jcv_real d = e1->a * e2->b - e1->b * e2->a;
	if( JCV_FABS(d) < (jcv_real)0.00001f )
	{
		return 0;
	}
	out->x = (e1->c * e2->b - e1->b * e2->c) / d;
	out->y = (e1->a * e2->c - e1->c * e2->a) / d;

	const jcv_edge* e;
	const jcv_halfedge* he;
	if( jcv_point_less( &e1->sites[1]->p, &e2->sites[1]->p) )
	{
		he = he1;
		e = e1;
	}
	else
	{
		he = he2;
		e = e2;
	}

	int right_of_site = out->x >= e->sites[1]->p.x;
	if ((right_of_site && he->direction == JCV_DIRECTION_LEFT) || (!right_of_site && he->direction == JCV_DIRECTION_RIGHT))
	{
		return 0;
	}

	return 1;
}


// Priority queue

static int jcv_pq_moveup(jcv_priorityqueue* pq, int pos)
{
	void* node = pq->items[pos];

	for( int parent = (pos >> 1);
		 pos > 1 && pq->compare(pq->items[parent], node);
		 pos = parent, parent = parent >> 1)
	{
		pq->items[pos] = pq->items[parent];
		pq->setpos( (void*)pq->items[pos], pos );
	}

	pq->items[pos] = node;
	pq->setpos( (void*)pq->items[pos], pos );
	return pos;
}

static int jcv_pq_maxchild(jcv_priorityqueue* pq, int pos)
{
	int child = pos << 1;
	if( child >= pq->numitems )
		return 0;
	if( (child + 1) < pq->numitems && pq->compare(pq->items[child], pq->items[child+1]) )
		return child+1;
	return child;
}

static int jcv_pq_movedown(jcv_priorityqueue* pq, int pos)
{
	void* node = pq->items[pos];

	int child;
	while( (child = jcv_pq_maxchild(pq, pos)) &&
			pq->compare( node, pq->items[child] ) )
	{
		pq->items[pos] = pq->items[child];
		pq->setpos( (void*)pq->items[pos], pos );
		pos = child;
	}

	pq->items[pos] = node;
	pq->setpos( (void*)pq->items[pos], pos );
	return pos;
}

static void jcv_pq_create(jcv_priorityqueue* pq, int capacity, void** buffer,
				FJCVPriorityQueueCompare cmp,
				FJCVPriorityQueueSetpos setpos,
				FJCVPriorityQueueGetpos getpos)
{
	pq->compare 	= cmp;
	pq->setpos		= setpos;
	pq->getpos		= getpos;
	pq->maxnumitems = capacity;
	pq->numitems	= 1;
	pq->items 		= buffer;
}

static int jcv_pq_empty(jcv_priorityqueue* pq)
{
	return pq->numitems == 1 ? 1 : 0;
}

static int jcv_pq_push(jcv_priorityqueue* pq, void* node)
{
	int n = pq->numitems++;
	pq->items[n] = node;
	return jcv_pq_moveup(pq, n);
}

static void* jcv_pq_pop(jcv_priorityqueue* pq)
{
	if( pq->numitems == 1 )
		return 0;

	void* node = pq->items[1];
	pq->items[1] = pq->items[--pq->numitems];
	jcv_pq_movedown(pq, 1);
	return node;
}

static void* jcv_pq_top(jcv_priorityqueue* pq)
{
	if( pq->numitems == 1 )
		return 0;
	return pq->items[1];
}

static void jcv_pq_remove(jcv_priorityqueue* pq, const void* node)
{
	if( pq->numitems == 1 )
		return;
	int pos = pq->getpos(node);
	if( pos == 0 )
		return;

	pq->items[pos] = pq->items[--pq->numitems];
	if( pq->compare( node, pq->items[pos] ) )
		jcv_pq_moveup( pq, pos );
	else
		jcv_pq_movedown( pq, pos );
	pq->setpos( (const void*)node, 0 );
}

// internal functions

static inline jcv_site* jcv_nextsite(jcv_context_internal* internal)
{
	return (internal->currentsite < internal->numsites) ? &internal->sites[internal->currentsite++] : 0;
}

static jcv_halfedge* jcv_get_edge_above_x(jcv_context_internal* internal, const jcv_point* p)
{
	// Gets the arc on the beach line at the x coordinate (i.e. right above the new site event)

	// A good guess it's close by (Can be optimized)
	jcv_halfedge* he = internal->last_inserted;
    if( !he )
    {
        if( p->x < internal->width / 2 )
            he = internal->beachline_start;
        else
            he = internal->beachline_end;
    }

	//
	if( he == internal->beachline_start || (he != internal->beachline_end && jcv_halfedge_rightof(he, p)) )
	{
		do {
			he = he->right;
		}
		while( he != internal->beachline_end && jcv_halfedge_rightof(he, p) );

		he = he->left;
	}
	else
	{
		do {
			he = he->left;
		}
		while( he != internal->beachline_start && !jcv_halfedge_rightof(he, p) );
	}

	return he;
}

static int jcv_check_circle_event(jcv_halfedge* he1, jcv_halfedge* he2, jcv_point* vertex)
{
	jcv_edge* e1 = he1->edge;
	jcv_edge* e2 = he2->edge;
	if( e1 == 0 || e2 == 0 || e1->sites[1] == e2->sites[1] )
	{
		return 0;
	}

	return jcv_halfedge_intersect(he1, he2, vertex);
}

static void jcv_site_event(jcv_context_internal* internal, jcv_site* site)
{
	jcv_halfedge* left 	 = jcv_get_edge_above_x(internal, &site->p);
	jcv_halfedge* right	 = left->right;
	jcv_site*	  bottom = jcv_halfedge_rightsite(left);
	if( !bottom )
		bottom = internal->bottomsite;

	jcv_edge* edge = jcv_edge_new(internal, bottom, site);
	edge->next = internal->edges;
	internal->edges = edge;

	jcv_halfedge* edge1 = jcv_halfedge_new(internal, edge, JCV_DIRECTION_LEFT);
	jcv_halfedge* edge2 = jcv_halfedge_new(internal, edge, JCV_DIRECTION_RIGHT);

	jcv_halfedge_link(left, edge1);
	jcv_halfedge_link(edge1, edge2);

	internal->last_inserted = edge1;

	jcv_point p;
	if( jcv_check_circle_event( left, edge1, &p ) )
	{
		jcv_pq_remove(internal->eventqueue, left);
		left->vertex 	= p;
		left->y		 	= p.y + jcv_point_dist(&site->p, &p);
		jcv_pq_push(internal->eventqueue, left);
	}
	if( jcv_check_circle_event( edge2, right, &p ) )
	{
		edge2->vertex	= p;
		edge2->y		= p.y + jcv_point_dist(&site->p, &p);
		jcv_pq_push(internal->eventqueue, edge2);
	}
}

static inline jcv_real jcv_determinant(const jcv_point* a, const jcv_point* b, const jcv_point* c)
{
	return (b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x);
}

static jcv_real jcv_calc_sort_metric(const jcv_site* site, const jcv_graphedge* edge)
{
	jcv_real diffy = edge->pos[0].y - site->p.y;
	jcv_real angle = JCV_ATAN2( diffy, edge->pos[0].x - site->p.x );
	if( diffy < 0 )
		angle = 2 * (jcv_real)3.14159265358979323846264338327950288f + angle;
	return angle;
}

static void jcv_sortedges_insert(jcv_graphedge** sortedlist, jcv_graphedge* edge)
{
    // Special case for the head end
    if (*sortedlist == 0 || (*sortedlist)->angle >= edge->angle)
    {
        edge->next = *sortedlist;
        *sortedlist = edge;
    }
    else
    {
        // Locate the node before the point of insertion
    	jcv_graphedge* current = *sortedlist;
        while(current->next != 0 && current->next->angle < edge->angle)
        {
            current = current->next;
        }
        edge->next = current->next;
        current->next = edge;
    }
}

static void jcv_finishline(jcv_context_internal* internal, jcv_edge* e)
{
	if( !jcv_edge_clipline( e, internal->width, internal->height) )
		return;

	if( e->pos[0].x == e->pos[1].x && e->pos[0].y == e->pos[1].y )
		return;

	// Make sure the graph edges are CCW
	int flip = jcv_determinant(&e->sites[0]->p, &e->pos[0], &e->pos[1]) > (jcv_real)0 ? 0 : 1;

	for( int i = 0; i < 2; ++i )
	{
        jcv_graphedge* ge = jcv_alloc_graphedge(internal);

		ge->edge = e;
		ge->next = 0;
		ge->neighbor = e->sites[1-i];
		ge->pos[flip] = e->pos[i];
		ge->pos[1-flip] = e->pos[1-i];
		ge->angle = jcv_calc_sort_metric(e->sites[i], ge);

		jcv_sortedges_insert( &e->sites[i]->edges, ge );

		// check that we didn't accidentally add a duplicate (rare), then remove it
		if( ge->next && ge->angle == ge->next->angle )
		{
			if( jcv_point_eq( &ge->pos[0], &ge->next->pos[0] ) && jcv_point_eq( &ge->pos[1], &ge->next->pos[1] ) )
			{
				ge->next = ge->next->next; // Throw it away, they're so few anyways
			}
		}
	}
}


static void jcv_endpos(jcv_context_internal* internal, jcv_edge* e, const jcv_point* p, int direction)
{
	e->pos[direction] = *p;

	if( e->pos[0].x != JCV_INVALID_VALUE && e->pos[1].x != JCV_INVALID_VALUE )
	{
		jcv_finishline(internal, e);
	}
}

static inline void jcv_create_corner_edge(jcv_context_internal* internal, const jcv_site* site, jcv_graphedge* current, jcv_graphedge* gap)
{
	gap->neighbor	= 0;
	gap->pos[0]		= current->pos[1];

	if( current->pos[1].x < internal->width && current->pos[1].y == (jcv_real)0 )
	{
		gap->pos[1].x = internal->width;
		gap->pos[1].y = (jcv_real)0;
	}
	else if( current->pos[1].x > (jcv_real)0 && current->pos[1].y == internal->height )
	{
		gap->pos[1].x = (jcv_real)0;
		gap->pos[1].y = internal->height;
	}
	else if( current->pos[1].y > (jcv_real)0 && current->pos[1].x == (jcv_real)0 )
	{
		gap->pos[1].x = (jcv_real)0;
		gap->pos[1].y = (jcv_real)0;
	}
	else if( current->pos[1].y < internal->height && current->pos[1].x == internal->width )
	{
		gap->pos[1].x = internal->width;
		gap->pos[1].y = internal->height;
	}

	gap->angle = jcv_calc_sort_metric(site, gap);
}

// Since the algorithm leaves gaps at the borders/corner, we want to fill them
static void jcv_fillgaps(jcv_diagram* diagram)
{
	jcv_context_internal* internal = diagram->internal;
	for( int i = 0; i < internal->numsites; ++i )
	{
		jcv_site* site = &internal->sites[i];

		// They're sorted CCW, so if the current->pos[1] != next->pos[0], then we have a gap
		jcv_graphedge* current = site->edges;
		jcv_graphedge* next = current->next;
		if( !next )
		{
			// Only one edge, then we assume it's a corner gap
			jcv_graphedge* gap = jcv_alloc_graphedge(diagram->internal);
			gap->edge = current->edge; // not really true (should be 0)
			jcv_create_corner_edge(internal, site, current, gap);

			gap->next = current->next;
			current->next = gap;
			current = gap;
			next = site->edges;
		}
		while( current && next )
		{
			//if( jcv_point_on_edge(&current->pos[1], internal->width, internal->height) && jcv_point_dist_sq(&current->pos[1], &next->pos[0]) > (jcv_real)1 )
			if( jcv_point_on_edge(&current->pos[1], internal->width, internal->height) && !jcv_point_eq(&current->pos[1], &next->pos[0]) )
			{
				// Border gap
				if( current->pos[1].x == next->pos[0].x || current->pos[1].y == next->pos[0].y)
				{
					jcv_graphedge* gap = jcv_alloc_graphedge(diagram->internal);
					gap->edge 		= current->edge;
					gap->neighbor 	= 0;
					gap->pos[0]		= current->pos[1];
					gap->pos[1]		= next->pos[0];
					gap->angle 		= jcv_calc_sort_metric(site, gap);

					gap->next = current->next;
					current->next = gap;
				}
				else // Corner gap
				{
					jcv_graphedge* gap = jcv_alloc_graphedge(diagram->internal);
					gap->edge = current->edge; // not really true (should be 0)
					jcv_create_corner_edge(internal, site, current, gap);
					gap->next = current->next;
					current->next = gap;
				}
			}

			current = current->next;
			if( current )
			{
				next = current->next;
				if( !next )
					next = site->edges;
			}
		}
	}
}


static void jcv_circle_event(jcv_context_internal* internal)
{
	jcv_halfedge* left 		= (jcv_halfedge*)jcv_pq_pop(internal->eventqueue);

	jcv_halfedge* leftleft 	= left->left;
	jcv_halfedge* right		= left->right;
	jcv_halfedge* rightright= right->right;
	jcv_site* bottom = jcv_halfedge_leftsite(left);
	jcv_site* top 	 = jcv_halfedge_rightsite(right);

	jcv_point vertex = left->vertex;
	jcv_endpos(internal, left->edge, &vertex, left->direction);
	jcv_endpos(internal, right->edge, &vertex, right->direction);

	if( internal->last_inserted == left )
		internal->last_inserted = leftleft;
	else if( internal->last_inserted == right )
		internal->last_inserted = rightright;

	jcv_pq_remove(internal->eventqueue, right);
	jcv_halfedge_unlink(left);
	jcv_halfedge_unlink(right);
    jcv_halfedge_delete(internal, left);
    jcv_halfedge_delete(internal, right);

	int direction = JCV_DIRECTION_LEFT;
	if( bottom->p.y > top->p.y )
	{
		jcv_site* temp = bottom;
		bottom = top;
		top = temp;
		direction = JCV_DIRECTION_RIGHT;
	}

	jcv_edge* edge = jcv_edge_new(internal, bottom, top);
	edge->next = internal->edges;
	internal->edges = edge;

	jcv_halfedge* he = jcv_halfedge_new(internal, edge, direction);
	jcv_halfedge_link(leftleft, he);
	jcv_endpos(internal, edge, &vertex, JCV_DIRECTION_RIGHT - direction);

	jcv_point p;
	if( jcv_check_circle_event( leftleft, he, &p ) )
	{
		jcv_pq_remove(internal->eventqueue, leftleft);
		leftleft->vertex 	= p;
		leftleft->y		 	= p.y + jcv_point_dist(&bottom->p, &p);
		jcv_pq_push(internal->eventqueue, leftleft);
	}
	if( jcv_check_circle_event( he, rightright, &p ) )
	{
		he->vertex 		= p;
		he->y		 	= p.y + jcv_point_dist(&bottom->p, &p);
		jcv_pq_push(internal->eventqueue, he);
	}
}

void jcv_diagram_generate( int num_points, const jcv_point* points, int width, int height, jcv_diagram* d )
{
	jcv_diagram_generate_useralloc(num_points, points, width, height, 0, jcv_alloc_fn, jcv_free_fn, d);
}

typedef union _jcv_cast_align_struct
{
	char*	charp;
	void**	voidpp;
} jcv_cast_align_struct;

void jcv_diagram_generate_useralloc( int num_points, const jcv_point* points, int width, int height, void* userallocctx, FJCVAllocFn allocfn, FJCVFreeFn freefn, jcv_diagram* d )
{
	if( d->internal )
		jcv_diagram_free( d );

	int max_num_events = (int)(sqrtf(num_points)) * 4;
	size_t sitessize = (size_t)num_points * sizeof(jcv_site);
	size_t memsize = 8u + (size_t)max_num_events * sizeof(void*) + sizeof(jcv_priorityqueue) + sitessize + sizeof(jcv_context_internal);

	char* originalmem = (char*)allocfn(userallocctx, memsize);
	memset(originalmem, 0, memsize);

	// align memory
	char* mem = originalmem + 8 - ( (size_t)(originalmem) & 0x7);

	jcv_context_internal* internal = (jcv_context_internal*)mem;
	mem += sizeof(jcv_context_internal);

	internal->mem	 = originalmem;
	internal->memctx = userallocctx;
	internal->alloc  = allocfn;
	internal->free   = freefn;

	internal->beachline_start = jcv_alloc_halfedge(internal);
	internal->beachline_end	= jcv_alloc_halfedge(internal);

	internal->beachline_start->left 	= 0;
	internal->beachline_start->right 	= internal->beachline_end;
	internal->beachline_end->left		= internal->beachline_start;
	internal->beachline_end->right	= 0;

	internal->last_inserted = 0;

	internal->sites = (jcv_site*) mem;
	mem += sitessize;

	internal->eventqueue = (jcv_priorityqueue*)mem;
	mem += sizeof(jcv_priorityqueue);

	jcv_cast_align_struct tmp;
	tmp.charp = mem;
	internal->eventmem = tmp.voidpp;

	jcv_pq_create(internal->eventqueue, max_num_events, (void**)internal->eventmem,
						(FJCVPriorityQueueCompare)jcv_halfedge_compare,
						(FJCVPriorityQueueSetpos)jcv_halfedge_setpos,
						(FJCVPriorityQueueGetpos)jcv_halfedge_getpos);

	for( int i = 0; i < num_points; ++i )
	{
		internal->sites[i].p 		= points[i];
		internal->sites[i].edges	= 0;
		internal->sites[i].index	= i;
	}

	// Remove duplicates, to avoid anomalies
	qsort(internal->sites, (size_t)num_points, sizeof(jcv_site), jcv_point_cmp);

	int offset = 0;
	for (int is = 1; is < num_points; is++)
	{
		if( internal->sites[is].p.y == internal->sites[is - 1].p.y && internal->sites[is].p.x == internal->sites[is - 1].p.x )
		{
			offset++;
			continue;
		}
		else if (offset > 0)
		{
			internal->sites[is - offset] = internal->sites[is];
		}
	}
	num_points -= offset;

	d->internal = internal;
	d->width	= width;
	d->height	= height;
	d->numsites = num_points;

	internal->width 		= width;
	internal->height 		= height;
	internal->numsites 		= num_points;
	internal->numsites_sqrt	= (int)(sqrtf(num_points));
	internal->currentsite 	= 0;

	internal->bottomsite = jcv_nextsite(internal);

	jcv_site* site = jcv_nextsite(internal);

	while( 1 )
	{
		jcv_point lowest_pq_point;
		if( !jcv_pq_empty(internal->eventqueue) )
		{
			jcv_halfedge* he = (jcv_halfedge*)jcv_pq_top(internal->eventqueue);
			lowest_pq_point.x = he->vertex.x;
			lowest_pq_point.y = he->y;
		}

		if( site != 0 && (jcv_pq_empty(internal->eventqueue) || jcv_point_less(&site->p, &lowest_pq_point) ) )
		{
			jcv_site_event(internal, site);
			site = jcv_nextsite(internal);
		}
		else if( !jcv_pq_empty(internal->eventqueue) )
		{
			jcv_circle_event(internal);
		}
		else
		{
			break;
		}
	}

	for( jcv_halfedge* he = internal->beachline_start->right; he != internal->beachline_end; he = he->right )
	{
		jcv_finishline(internal, he->edge);
	}

	jcv_fillgaps(d);
}

#endif // JC_VORONOI_IMPLEMENTATION


