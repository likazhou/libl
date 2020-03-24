#if GUI_ENABLE





//External Variables
extern const u8 disp_aHideMenuKey[8];
extern const t_disp_list tbl_menu_root[];
extern const u8 disp_tblrootrow;
extern const t_disp_list tbl_menu_hide[];
extern const u8 disp_tblhiderow;



static int disp_ReadKey()
{	
	int i, nKey;
	
	for (i = 0; i < 10; i++)
	{
		nKey = key_Read();
		if (nKey)
			return nKey;
		os_thd_sleep(100);
	}
	return 0;
}




//清屏
void disp_ClrDispArea()
{

	gui_DrawRect_Fill(0, DISP_ROW_0, LCD_X_MAX - 1, LCD_Y_MAX -1, COLOR_WHITE);
}

void disp_SingleData(t_disp *p, int nRow, const char *pItem, const char *pData, int nSel)
{
	int nX, nY;
	t_color nColor;

	nY = DISP_ROW_0 + nRow * DISP_ROW_ITV;
	//选中反色
	if (p->idx[p->mdeep] == nSel)
		nColor = COLOR_WHITE;
	else
		nColor = COLOR_BLACK;

	nX = gui_DrawString_Mixed(DISP_STATE_X, nY, pItem, nColor);

	if (pData != NULL)
		gui_DrawString_Mixed(nX, nY, pData, nColor);
}

void disp_SingleData_Center(t_disp *p, int nRow, const char *pData, int nSel)
{
	int nY;
	t_color nColor;

	nY = DISP_ROW_0 + nRow * DISP_ROW_ITV;
	//选中反色
	if (p->idx[p->mdeep] == nSel)
		nColor = COLOR_WHITE;
	else
		nColor = COLOR_BLACK;
	
	gui_DrawString_Mixed_Center(DISP_STATE_X, nY, pData, nColor);
}

void disp_SingleData_SeparateAlign(t_disp *p, int nRow, const char *pItem, const char *pData, int nSel)
{
	int nY;
	t_color nColor;

	nY = DISP_ROW_0 + nRow * DISP_ROW_ITV;
	//选中反色
	if (p->idx[p->mdeep] == nSel)
		nColor = COLOR_WHITE;
	else
		nColor = COLOR_BLACK;
	
	gui_DrawString_Mixed(0, nY, pItem, nColor);

	if (pData != NULL)
		gui_DrawString_Mixed_Right(nY, pData, nColor);
}


void disp_Main(t_disp *p, int nInit)
{
	t_disp_list *pParent;
	void (*pfHandler)(t_disp *, void *, int);
	int i, j, nKey, nSize, nType, nSel;
	u8 aKey[8];

	if (nInit) {
		memset(p, 0, sizeof(t_disp));
		p->tn = 1;
		p->tmo = DISP_IDLE_SIZE;
	}
	
	for (; ; p->tmo++)
	{
		nKey = disp_ReadKey();
		if (nKey)
		{
			p->key = nKey;
			
			disp_BglOn();
			p->tmo = 0;
			//隐藏菜单
			if (p->mdeep < 2)
			{
				memmove(&aKey[0], &aKey[1], 7);
				aKey[7] = p->key;
				if (memcmp(aKey, disp_aHideMenuKey, 8) == 0)
				{
					p->hide = 1;
					p->mdeep = 1;
				}
			}
		}
		else
		{
			p->key = DISP_KEY_NULL;

			if (p->tmo >= DISP_IDLE_SIZE)
			{
				p->tmo = 0;
				p->mdeep = 1;
				p->key = DISP_KEY_ESC;
				p->ste = 0;
				disp_BglOff();
			}
		}

		disp_ClrDispArea();

		//键盘决策
		nSel = 0;
		switch (p->key)
		{
		case DISP_KEY_UP:
			nSel = -1;
			break;
		case DISP_KEY_DOWN:
			nSel = 1;
			break;
		case DISP_KEY_ESC:
			if (p->pw)
			{
				p->pw = 0;
			}
			else
			{
				if (p->mdeep)
				{
					p->mdeep -= 1;
					p->sdeep = 0;
				}
			}
			break;
		case DISP_KEY_ENTER:
			if (p->mdeep < 3)
			{
				p->mdeep += 1;
				p->idx[p->mdeep] = 0;
			}
			break;
#if DISP_KEY_UNLOCK_ENABLE
		case DISP_KEY_UNLOCK:
			p->ste ^= 1;
			p->pw = 0;
			break;
#endif
		default:
			break;
		}

		if (p->mdeep == 0)
		{
			disp_MainUI(p);
			p->hide = 0;
		}
		else
		{
			if (p->hide)
			{
				pParent = &tbl_menu_hide[0];
				nSize = disp_tblhiderow;
			}
			else
			{
				pParent = &tbl_menu_root[0];
				nSize = disp_tblrootrow;
			}
			
			nType = DISP_LIST_T_MENU;
			for (i = 1; ; i++)
			{
				p->node = pParent + p->idx[i];
				if (nType == DISP_LIST_T_MENU)
				{
					if (i >= p->mdeep)
					{
						//绘制当前深度菜单
						p->idx[p->mdeep] = cycle(p->idx[p->mdeep], 0, nSize - 1, nSel);
						if (p->idx[p->mdeep] < DISP_MAX_ROW)
						{
							nSize = MIN(nSize, DISP_MAX_ROW);
							j = 0;
						}
						else
						{
							nSize = p->idx[p->mdeep] + 1;
							j = nSize - DISP_MAX_ROW;
						}
						
						for (i = 0; j < nSize; i++, j++)
						{
							p->node = pParent + j;
							disp_SingleData(p, i, p->node->str, NULL, j);
						}
						break;
					}
					pParent = p->node->next;
					nSize = p->node->size;
					nType = p->node->type;
				}
				
				if (nType == DISP_LIST_T_SET)
				{
					if (p->pw)
					{
						if (disp_InputPassword(p, nSel) != SYS_R_OK)
							break;
					}
					
					if (p->sdeep == 1)
					{
						if (p->key == DISP_KEY_ENTER)
						{
							if (p->ste)
							{
								p->sdeep = 2;
							}
							else
							{
								disp_InputPassword(p, nSel);
								break;
							}
						}
					}
					
					if (p->sdeep == 0)
						p->sdeep = 1;
					
					if ((p->sdeep > 1) && (p->sdeep < DISP_SDEEP_MAX))
					{
#if DISP_KEY_LR_ENABLE
						if (p->key == DISP_KEY_LEFT)
							p->sdeep -= 1;
						if (p->key == DISP_KEY_RIGHT)
							p->sdeep += 1;
#else
						if (p->key == DISP_KEY_UP)
						{
							p->sdeep += 1;
							nSel = 0;
						}
#endif
					}
				}
				
				if (nType != DISP_LIST_T_MENU)
				{
					pfHandler = (void (*)(t_disp *, void *, int))(p->node->next);
					(*pfHandler)(p, (void *)i, nSel);
					break;
				}
			}
		}
		
		disp_StatusBar(p);
		
		gui_Redraw();
	}
}


#endif

