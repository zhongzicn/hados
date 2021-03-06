/*
 HADOS
 High Availability Distributed Search Engine
 Copyright (C) 2013 Jaeksoft / Emmanuel Keller

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ============================================================================
 Name        : tempfile.c
 Author      : Emmanuel Keller
 Version     :
 Copyright   : Jaeksoft / Emmanuel Keller
 ============================================================================
 */

#include "hados.h"

int hados_tempfile_new(struct hados_tempfile *tempfile,
		struct hados_context *context) {
	tempfile->path = malloc((strlen(context->temp_dir) + 60) * sizeof(char));
	sprintf(tempfile->path, "%s/%p", context->temp_dir, tempfile->path);
	return HADOS_SUCCESS;
}

void hados_tempfile_free(struct hados_tempfile *tempfile) {
	if (tempfile->path != NULL ) {
		unlink(tempfile->path);
		free(tempfile->path);
		tempfile->path = NULL;
	}
}

int hados_tempfile_upload(struct hados_tempfile *tempfile,
		struct hados_context *context) {
	int res = HADOS_INTERNAL_ERROR;
	FILE *fd = fopen(tempfile->path, "wb");
	if (fd == NULL )
		return hados_response_set_errno(&context->response);
	const char* content_length = hados_context_get_env(context,
			"CONTENT_LENGTH");
	long contentLength = 0;
	if (content_length != NULL )
		contentLength = atol(content_length);
	if (contentLength == 0) {
		res = hados_response_set_status(&context->response,
				HADOS_NO_CONTENT_LENGTH_GIVEN, "Missing content length header");
		goto exit;
	}
	int c;
	context->bytes_received = 0;
	for (;;) {
		c = FCGX_GetChar(context->fcgxRequest.in);
		if (c == EOF) {
			break;
		}
		context->bytes_received++;
		if (fputc(c, fd) == EOF) {
			hados_response_set_errno(&context->response);
			break;
		}
	}
	if (context->bytes_received != contentLength)
		res = hados_response_set_status(&context->response,
				HADOS_NOT_ENOUGH_BYTES_RECEIVED, "Not enough bytes received");

	res = HADOS_SUCCESS;

	exit:

	if (fd != NULL )
		fclose(fd);

	return res;
}

