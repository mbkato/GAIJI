//! 漢字文字列の印字
/*!
 * \param	szString	印字する文字列のポインタ。
 * \param	slen		印字する文字列の長さ。
*/
void CGxPrinter::TextPutK(LPCSTR szString, int slen)
{
	if (m_hPrintDC)
	{
		COLORREF oldCL = ::GetTextColor(m_hPrintDC);
		if (m_dwOptTextPutHL & OPT_HLPUTX_LIGHT)
			::SetTextColor(m_hPrintDC,m_PutColorTbl[PUTCL_LIGHT]);
		if (m_dwOptTextPutHL & OPT_HLPUTX_HDRFACE)
			::SetTextColor(m_hPrintDC,m_PutColorTbl[PUTCL_HDRFACE]);
		if (m_dwOptTextPutHL & OPT_HLPUTX_MUINUM)
			::SetTextColor(m_hPrintDC,m_PutColorTbl[PUTCL_MUINUM]);

		int FontZen  = m_CurentKnjFontType+1;
		int FontType = SelectFont(m_CurentKnjFontType);
		if (m_bSO)
		{
			m_CurentFontType += dotFONT_MAX;
			FontZen			 += dotFONT_MAX;
		}
		HFONT hOldFont = (HFONT)::SelectObject(m_hPrintDC, m_DotLogFontTbl[m_CurentFontType].hFont);
		LPCSTR psz = szString;
		if (slen == -1)
			slen = INT_MAX;

		int nHight = m_DotLogFontTbl[m_CurentFontType].ldfCPMY;
		int nCPPY = m_DotLogFontTbl[m_CurentFontType].ldfCPPY;
		int AlgCY  = 0;
		if (nCPPY < m_LCPichCY)
		{
			TEXTMETRIC tm;
			::GetTextMetrics(m_hPrintDC,&tm);
			AlgCY = ((m_LCPichCY - ((tm.tmAscent) * 10)) / 2) /10;
		}
		int startX = 0;
		CRect rect;
		while(*psz && slen)
		{
			startX = LOCATEX();
			if (_ismbblead( (unsigned int)*psz ))
			{
				HFONT hPush = (HFONT)::SelectObject(m_hPrintDC,m_DotLogFontTbl[FontZen].hFont);
				int ch = MAKEWORD(*(psz+1),*psz);
				
				//
				// 文字コードが外字領域だったら、赤丸で描画。
				//
				if (0xF040 <= ch && ch <= 0xF9FC)
				{
					CDC mgDC;
					mgDC.Attach(m_hPrintDC);

					mgDC.TextOut(LOCATEX(),LOCATEY()-AlgCY,"劇");

					CPen penArc;
					CPen* pOldPen;
					CBrush nulBrh;
					CBrush* pOldBrh;

					penArc.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
					LOGBRUSH lb;
					lb.lbColor = RGB(255,255,255);
					lb.lbHatch = NULL;
					lb.lbStyle = BS_NULL;
					nulBrh.CreateBrushIndirect(&lb);
					
					pOldPen = mgDC.SelectObject(&penArc);
					pOldBrh = mgDC.SelectObject(&nulBrh);

					int LX = LOCATEX();
					int LY = LOCATEY()-AlgCY;

					CRect rc(LX,LY,
						LX+((m_DotLogFontTbl[m_CurentFontType].ldfCPPX*2)/10),
						LY-(m_LCPichCY/10));
					
					mgDC.Ellipse(rc);

					mgDC.SelectObject(pOldPen);
					mgDC.SelectObject(pOldBrh);
					mgDC.Detach();
				}
				else
				{
					//
					// 普通の文字コードはこっち
					//
					::TextOut(m_hPrintDC,LOCATEX(),LOCATEY()-AlgCY,psz,2);
				}
				::SelectObject(m_hPrintDC,hPush);
				CharNext();
				CharNext();
				psz += 2;
				slen -= 2;
			}
			else
			{
				::TextOut(m_hPrintDC,LOCATEX(),LOCATEY()-AlgCY,psz,1);
				CharNext();
				psz += 1;
				slen -= 1;
			}
			if (m_bUnderLine)
			{
				if (m_dwOptTextPutHL & OPT_HLPUTX_BGTONE || 
					m_dwOptTextPutHL & 0x0000FF00 ||
					m_dwOptTextPutHL & OPT_HLPUTX_VLINE)
				{
					rect.top    = (LOCATEY()-(m_LCPichCY / 10));
				}
				else
					rect.top    = (LOCATEY()-AlgCY - nHight)+2;
				rect.left   = startX;
				rect.right  = LOCATEX()+1;
				::MoveToEx(m_hPrintDC,rect.left,rect.top,NULL);
				::LineTo(m_hPrintDC, rect.right,rect.top);
			}
		}
		if (m_bSO)
			m_CurentFontType -= dotFONT_MAX;
		::SelectObject(m_hPrintDC, hOldFont);
		::SetTextColor(m_hPrintDC,oldCL);
		SelectFont(FontType);
	}
}

