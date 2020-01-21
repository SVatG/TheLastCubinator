/* Returns the sign of x */
float sgnf ( float x ) {
	if ( x < 0 ) {
		return -1;
	}
	if ( x > 0 ) {
		return 1;
	}
	return 0;
}

/* Returns the absolute value of x */
float absf ( float x ) {
	if ( x < 0 ) {
		return -x;
	}
	return x;
}

/* sqC (v, n)
* This function implements the c(v,n) utility function
*
* c(v,n) = sgnf(cos(v)) * |cos(v)|^n
*/
float sqC ( float v, float n ) {
	return sgnf((float)cos(v)) * (float)powf(absf((float)cos(v)),n);
}

/* sqCT (v, n, alpha)
* This function implements the CT(v,n,alpha) utility function
*
* CT(v,n,alpha) = alpha + c(v,n)
*/
float sqCT ( float v, float n, float alpha ) {
	return alpha + sqC(v,n);
}

/* sqS (v, n)
* This function implements the s(v,n) utility function
*
* s(v,n) = sgnf(sin(v)) * |sin(v)|^n
*/
float sqS ( float v, float n ) {
	return sgnf((float)sin(v)) * (float)powf(absf((float)sin(v)),n);
}

/* sqEllipsoid(a1, a2, a3, u, v, n, e, *x, *y, *z, *nx, *ny, *nz)
 *
 * a1, a2, and a3 are the x, y, and z scaling factors, respecfully.
 * For proper generation of the solid, u should be >= -PI / 2 and <= PI / 2.
 * Similarly, v should be >= -PI and <= PI.
 */
void sqEllipsoid ( float a1, float a2, float a3, float u, float v, float n,
	float e, float *x, float  *y, float *z, float *nx, float *ny,
	float *nz ) {
	*x = a1 * sqC (u, n) * sqC (v, e);
	*y = a2 * sqC (u, n) * sqS (v, e);
	*z = a3 * sqS (u, n);
	*nx= sqC (u, 2 - n) * sqC (v, 2 - e) / a1;
	*ny= sqC (u, 2 - n) * sqS (v, 2 - e) / a2;
	*nz= sqS (u, 2 - n) / a3;
}

/* sqToroid(a1, a2, a3, u, v, n, e, alpha, *x, *y, *z, *nx, *ny, *nz)
 *
 * a1, a2, and a3 are the x, y, and z scaling factors, respecfully.
 * For proper generation of the solid, u should be >= -PI and <= PI.
 * Similarly, v should be >= -PI and <= PI.
 * Also, alpha should be > 1.
 */
void sqToroid ( float a1, float a2, float a3, float u, float v, float n,
	float e, float alpha, float *x, float  *y, float *z, float *nx,
	float *ny, float *nz ) {
	float A1, A2, A3;
	A1 = 1 / (a1 + alpha);
	A2 = 1 / (a2 + alpha);
	A3 = 1 / (a3 + alpha);
	*x = A1 * sqCT (u, e, alpha) * sqC (v, n);
	*y = A2 * sqCT (u, e, alpha) * sqS (v, n);
	*z = A3 * sqS (u, e);
	*nx= sqC (u, 2 - e) * sqC (v, 2 - n) / A1;
	*ny= sqC (u, 2 - e) * sqS (v, 2 - n) / A2;
	*nz= sqS (u, 2 - e) / A3;
}

/* sqSolidEllipsoid ( sq, make_display_list, gen_texture_coordinates )
 *
 * Generates a solid ellipsoid using the parameters from sq and optionally
 * generates texture coordinates and a display list using the ID from sq.
 */
void sqSolidEllipsoid ( SuperQuadric *sq, int make_display_list, int gen_texture_coordinates ) {
	float U, dU, V, dV;
	float S, dS, T, dT;
	int X, Y;     /* for looping */
	float x, y, z;
	float nx, ny, nz;

	/* Calculate delta variables */
	dU = (float)(sq->u2 - sq->u1) / (float)sq->u_segs;
	dV = (float)(sq->v2 - sq->v1) / (float)sq->v_segs;
	dS = (float)(sq->s2 - sq->s1) / (float)sq->u_segs;
	dT = (float)(sq->t2 - sq->t1) / (float)sq->v_segs;

	/* If we're going to make a display list then start it */
	if ( make_display_list ) {
		glNewList ( sq->gl_list_id, GL_COMPILE );
	}

	/* Initialize variables for loop */
	U = sq->u1;
	S = sq->s1;
	glBegin ( GL_QUADS );
	for ( Y = 0; Y < sq->u_segs; Y++ ) {
		/* Initialize variables for loop */
		V = sq->v1;
		T = sq->t1;
		for ( X = 0; X < sq->v_segs; X++ ) {
			/* VERTEX #1 */
			sqEllipsoid ( 1, 1, 1, U, V, sq->n, sq->e, &x, &y, &z, &nx, &ny, &nz );
			glNormal3f ( nx, ny, nz );
			glTexCoord2f ( S, T );
			glVertex3f ( x, y, z );

			/* VERTEX #2 */
			sqEllipsoid ( 1, 1, 1, U + dU, V, sq->n, sq->e, &x, &y, &z, &nx, &ny, &nz );
			glNormal3f ( nx, ny, nz );
			glTexCoord2f ( S + dS, T );
			glVertex3f ( x, y, z );

			/* VERTEX #3 */
			sqEllipsoid ( 1, 1, 1, U + dU, V + dV, sq->n, sq->e, &x, &y, &z, &nx, &ny, &nz );
			glNormal3f ( nx, ny, nz );
			glTexCoord2f ( S + dS, T + dT );
			glVertex3f ( x, y, z );

			/* VERTEX #4 */
			sqEllipsoid ( 1, 1, 1, U, V + dV, sq->n, sq->e, &x, &y, &z, &nx, &ny, &nz );
			glNormal3f ( nx, ny, nz );
			glTexCoord2f ( S, T + dT );
			glVertex3f ( x, y, z );

			/* Update variables for next loop */
			V += dV;
			T += dT;
		}
		/* Update variables for next loop */
		S += dS;
		U += dU;
	}
	glEnd ( );

	/* If we're making a display list then stop */
	if ( make_display_list ) {
		glEndList ( );
	}
}
