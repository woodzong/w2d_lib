#include "w3d_comm.h"

void W_Mat_Mul_VECTOR4D_4X4(W_VECTOR4D_PTR  va,
	W_MATRIX4X4_PTR mb,
	W_VECTOR4D_PTR  vprod)
{
	// this function multiplies a VECTOR4D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function makes no assumptions

	for (int col = 0; col < 4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row = 0; row < 4; row++)
		{
			// add in next product pair
			sum += (va->M[row] * mb->M[row][col]);
		} // end for index

		  // insert resulting col element
		vprod->M[col] = sum;

	} // end for col

} // end Mat_Mul_VECTOR4D_4X4


void W_Mat_Mul_MATRIX_4X4(W_MATRIX4X4_PTR  ma, W_MATRIX4X4_PTR mb, W_MATRIX4X4_PTR  mprod)
{
	// this function multiplies two 4x4 matrices together and 
	// and stores the result in mprod
	// note later we will take advantage of the fact that we know
	// that w=1 always, and that the last column of a 4x4 is
	// always 0

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb

			float sum = 0; // used to hold result

			for (int index = 0; index < 4; index++)
			{
				// add in next product pair
				sum += (ma->M[row][index] * mb->M[index][col]);
			} // end for index

			  // insert resulting row,col element
			mprod->M[row][col] = sum;

		} // end for col

	} // end for row

} // end Mat_Mul_4X4


void W_Transform_RENDERLIST4DV1(W_RENDERLIST4DV1_PTR rend_list, W_MATRIX4X4_PTR mt, int coord_select)
{
	switch (coord_select)
	{
	case W_TRANSFORM_LOCAL_ONLY:
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			W_POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & W_POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & W_POLY4DV1_STATE_CLIPPED) || (curr_poly->state & W_POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}

			//
			for (int vertex = 0; vertex < 3; vertex++)
			{
				W_POINT4D presult;
				W_Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &presult);
				W_VECTOR4D_COPY(&curr_poly->vlist[vertex], &presult);
			}
		}

	}
	break;

	case W_TRANSFORM_TRANS_ONLY:
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			W_POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & W_POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & W_POLY4DV1_STATE_CLIPPED) || (curr_poly->state & W_POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}

			//
			for (int vertex = 0; vertex < 3; vertex++)
			{
				W_POINT4D presult;
				W_Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &presult);
				W_VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
			}
		}

	}
	break;

	case W_TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			W_POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
			if ((curr_poly == NULL) || !(curr_poly->state & W_POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & W_POLY4DV1_STATE_CLIPPED) || (curr_poly->state & W_POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}

			//
			for (int vertex = 0; vertex < 3; vertex++)
			{
				W_POINT4D presult;
				W_Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &(curr_poly->tvlist[vertex]));
				W_VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
			}
		}
	}
	break;

	default:
		break;

	}

}


void W_Transform_OBJECT4DV1(W_OBJECT4DV1_PTR obj, W_MATRIX4X4_PTR mt, int coord_select, int transform_basis)
{
	switch (coord_select)
	{
	case W_TRANSFORM_LOCAL_ONLY:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			W_POINT4D presult;

			W_Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);

			W_VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);
		}
	}
	break;

	case W_TRANSFORM_TRANS_ONLY:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			W_POINT4D presult;

			W_Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &presult);

			W_VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
		}
	}
	break;

	case W_TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			W_Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &obj->vlist_trans[vertex]);
		}
	}
	break;

	default:
		break;

	}

	if (transform_basis)
	{
		W_VECTOR4D vresult;

		//rotate ux
		W_Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		W_VECTOR4D_COPY(&obj->ux, &vresult);

		//rotate uy
		W_Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		W_VECTOR4D_COPY(&obj->uy, &vresult);

		//rotate uz
		W_Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
		W_VECTOR4D_COPY(&obj->uz, &vresult);
	}
}

void W_Model_To_World_OBJECT4DV1(W_OBJECT4DV1_PTR obj, int coord_select)
{
	if (coord_select == W_TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			W_VECTOR4D_Add(&obj->vlist_local[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
		}
	}
	else
	{
		for (int vertex = 0; vertex < obj->num_vertices; vertex++)
		{
			W_VECTOR4D_Add(&obj->vlist_trans[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
		}
	}
}

void W_Build_Model_To_World_MATRIX4X4(W_VECTOR4D_PTR vpos, W_MATRIX4X4_PTR m)
{
	W_Mat_Init_4X4(m, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vpos->x, vpos->y, vpos->z, 1);
}

void W_Model_To_World_RENDERLIST4DV1(W_RENDERLIST4DV1_PTR rend_list, W_POINT4D_PTR world_pos, int coord_select)
{
	if (coord_select == W_TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			W_POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

			if ((curr_poly == NULL) || !(curr_poly->state & W_POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & W_POLY4DV1_STATE_CLIPPED) || (curr_poly->state & W_POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}


			//
			for (int vertex = 0; vertex < 3; vertex++)
			{
				W_VECTOR4D_Add(&curr_poly->vlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
			}
		}
	}
	else //W_TRANSFORM_TRANS_ONLY
	{
		for (int poly = 0; poly < rend_list->num_polys; poly++)
		{
			W_POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];

			if ((curr_poly == NULL) || !(curr_poly->state & W_POLY4DV1_STATE_ACTIVE) ||
				(curr_poly->state & W_POLY4DV1_STATE_CLIPPED) || (curr_poly->state & W_POLY4DV1_STATE_BACKFACE))
			{
				continue;
			}


			//
			for (int vertex = 0; vertex < 3; vertex++)
			{
				W_VECTOR4D_Add(&curr_poly->tvlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
			}
		}
	}
}

void W_Init_CAM4DV1(W_CAM4DV1_PTR cam, int cam_attr, W_POINT4D_PTR cam_pos, W_VECTOR4D_PTR cam_dir, W_POINT4D_PTR cam_target,
	float near_clip_z, float far_clip_z, float fov, float viewport_width, float viewport_height)
{
	cam->attr = cam_attr;

	W_VECTOR4D_COPY(&cam->pos, cam_pos);
	W_VECTOR4D_COPY(&cam->dir, cam_dir);

	W_VECTOR4D_INITXYZ(&cam->u, 1, 0, 0);
	W_VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
	W_VECTOR4D_INITXYZ(&cam->n, 0, 0, 1);

	if (cam_target != NULL)
	{
		W_VECTOR4D_COPY(&cam->target, cam_target);
	}
	else
	{
		W_VECTOR4D_ZERO(&cam->target);
	}

	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;

	cam->viewplane_width = viewport_width;
	cam->viewplane_height = viewport_height;

	cam->viewport_center_x = (viewport_width - 1) / 2;
	cam->viewport_center_y = (viewport_height - 1) / 2;

	cam->aspect_ratio = (float)viewport_width / (float)viewport_height;

	W_MAT_IDENTITY_4X4( &cam->mcam );  
	W_MAT_IDENTITY_4X4(&cam->mper);
	W_MAT_IDENTITY_4X4(&cam->mscr);
}

int W3D_Test()
{
	return 101;
}