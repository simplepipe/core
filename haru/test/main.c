#include <kernel/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <haru/hpdf.h>

jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler (HPDF_STATUS   error_no,
               HPDF_STATUS   detail_no,
               void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

int main (int argc, char **argv)
{
    HPDF_Doc  pdf;
    char fname[256];
    HPDF_Page page;
    HPDF_Font title_font;
    HPDF_Font detail_font;
    HPDF_UINT page_height;
    HPDF_UINT page_width;
    HPDF_REAL pw;
    struct string *text = file_to_string("inner://res/text");
    const char *detail_font_name;

    strcpy (fname, argv[0]);
    strcat (fname, ".pdf");

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        ref_dec(&text->base);
        return 1;
    }

    if (setjmp(env)) {
        HPDF_Free (pdf);
        ref_dec(&text->base);
        return 1;
    }

    /* declaration for using Japanese encoding. */
    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    detail_font_name = HPDF_LoadTTFontFromFile (pdf, argv[1], HPDF_TRUE);

    /* add a new page object. */
    page = HPDF_AddPage (pdf);

    title_font = HPDF_GetFont (pdf, "Helvetica", NULL);
	
    detail_font = HPDF_GetFont (pdf, detail_font_name, "UTF-8");

    HPDF_Page_SetFontAndSize (page, title_font, 10);
	page_width = 500;
	page_height = 800;
	HPDF_Page_SetWidth (page, page_width);
    HPDF_Page_SetHeight (page, page_height);
    HPDF_Page_BeginText (page);
    HPDF_Page_SetFontAndSize (page, detail_font, 12);
	HPDF_Page_MoveTextPos(page, 0, 800 - 12);
	HPDF_Page_SetFontAndSize (page, detail_font, 12);
	HPDF_Page_ShowText (page, text->ptr);
	 HPDF_Page_EndText (page);
    /* move the position of the text to top of the page. 
    HPDF_Page_MoveTextPos(page, 10, 190);
    HPDF_Page_ShowText (page, detail_font_name);
    HPDF_Page_ShowText (page, " (");
    HPDF_Page_ShowText (page, HPDF_Font_GetEncodingName (detail_font));
    HPDF_Page_ShowText (page, ")");
	
    HPDF_Page_SetFontAndSize (page, detail_font, 15);
    HPDF_Page_MoveTextPos (page, 10, -20);
    HPDF_Page_ShowText (page, "abcdefghijklmnopqrstuvwxyz");
    HPDF_Page_MoveTextPos (page, 0, -20);
    HPDF_Page_ShowText (page, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    HPDF_Page_MoveTextPos (page, 0, -20);
    HPDF_Page_ShowText (page, "1234567890");
    HPDF_Page_MoveTextPos (page, 0, -20);
	
    HPDF_Page_SetFontAndSize (page, detail_font, 10);
    HPDF_Page_ShowText (page, text->ptr);
    HPDF_Page_MoveTextPos (page, 0, -18);

    HPDF_Page_SetFontAndSize (page, detail_font, 16);
    HPDF_Page_ShowText (page, text->ptr);
    HPDF_Page_MoveTextPos (page, 0, -27);

    HPDF_Page_SetFontAndSize (page, detail_font, 23);
    HPDF_Page_ShowText (page, text->ptr);
    HPDF_Page_MoveTextPos (page, 0, -36);

    HPDF_Page_SetFontAndSize (page, detail_font, 30);
    HPDF_Page_ShowText (page, text->ptr);
    HPDF_Page_MoveTextPos (page, 0, -36);

    pw = HPDF_Page_TextWidth (page, text->ptr);
    page_height = 210;
    page_width = pw + 200;

    HPDF_Page_SetWidth (page, page_width);
    HPDF_Page_SetHeight (page, page_height);

 
    HPDF_Page_EndText (page);

    HPDF_Page_SetLineWidth (page, 0.5);

    HPDF_Page_MoveTo (page, 10, page_height - 25);
    HPDF_Page_LineTo (page, page_width - 10, page_height - 25);
    HPDF_Page_Stroke (page);

    HPDF_Page_MoveTo (page, 10, page_height - 85);
    HPDF_Page_LineTo (page, page_width - 10, page_height - 85);
    HPDF_Page_Stroke (page);
    */

    HPDF_SaveToFile (pdf, fname);

    /* clean up */
    HPDF_Free (pdf);
    ref_dec(&text->base);

    return 0;
}

