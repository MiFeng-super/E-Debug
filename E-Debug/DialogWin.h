#pragma once

//
// E 窗口信息
//
typedef struct EWINDOWINFO
{
	DWORD WindowCount;				//窗口数量
	vector<DWORD> WindowID;			//窗口ID
	vector<HTREEITEM> WindowItem;	//窗口的根节点
	vector<DWORD> WindowAddr;		//窗口地址，编译会无效了
}* PEWINDOWINFO;

//
// 临时内存
//
typedef struct MEMTEMP
{
	DWORD size;
	BYTE* p;
}*PMEMTEMP;

//
// E Control List
//
typedef struct ECONTROLLIST
{
	DWORD Unknow1;
	DWORD Unknow2;
	DWORD ZeroByte1;
	DWORD ZeroByte2;

	DWORD ControlCount;				//控件数量
	DWORD ControlSize;				//控件信息总大小

	vector<DWORD> ControlID;		//控件ID
	vector<HTREEITEM> ControlItem;	//控件ID插入到 Tree 的节点	//方便程序处理添加的，真实中并没有
	vector<DWORD> Controloffset;	//控件信息偏移
	DWORD Unknow3;
	vector<BYTE*> ControlInfo;		//控件信息指针	//方便程序处理添加的，真实中并没有
	vector<DWORD> ControlInfoSize;	//控件信息大小	//方便程序处理添加的，真实中并没有
}* PECONTROLLIST;



#pragma pack(push)
#pragma pack(1)
//
// 控件-窗口
//
typedef struct Analy_WINDOW
{
	DWORD ChildWindowId;	//控件类型
	BYTE ZeroByte[0x1A];
	DWORD left;				//左边
	DWORD top;				//顶边
	DWORD width;			//宽度
	DWORD height;			//高度
	BYTE UnknowByte[0xC];
	DWORD MousePointerSize;	//自定义鼠标指针所占字节集的大小,如果是选项中的鼠标指针,则为4
	DWORD MousePointer;		
	//BYTE customMousePointer[MousePointerSize]; //自定义鼠标指针字节集,如果为选项中的鼠标指针,则此值不存在
							//选项中的鼠标指针特殊值,如果为自定义鼠标指针,则为-1,
							//默认型为0
							//标准箭头型 0x7F00,十字型0x7F03
							//文本编辑型 0x7F01,沙漏型0x7F02
							//箭头及问号型 0x7F8B,箭头及沙漏型0x7F8A
							//禁止符型0x7F88,四向箭头型0x7F86
							//北<->东箭头型0x7F83,北<->南箭头型0x7F85
							//北<->西箭头型0x7F82,西<->东箭头型0x7F84
							//向上箭头型0x7F04,手型0x7F89

	string tag;				//标记 如果为空,则占一个1字节
	DWORD ZeroByte1;		//
	DWORD visible_disable;	//二进制第一位为可视,1为真,0为假;第二位为禁止,1为真,0为假
	DWORD ZeroByte2;
	DWORD UnknowSize;		//下面这个字段的长度
	BYTE UnknowByte2[0x20];	//尚不清楚，而且长度有时候不一样。
							//如果加上 0x20 超过长度，则属性是默认的
							//CWinUnitIDManger::GetUnitData	//具体可以分析这个函数
	DWORD border;   //0为无边框
					//1为普通可调边框
					//2为普通固定边框
					//3为窄标题可调边框
					//4为窄标题固定边框
					//5为镜框式可调边框
					//6为镜框式固定边框

	DWORD BackPicMode;  //0为图片居左上
						//1为图片平铺
						//2为图片居
						//3为缩放图片
	DWORD BackColor;	//0xFF000000代表默认底色,否则为R<<6||G<<4||B<<2||0;
	DWORD MaxBtn;		//最大化按钮,1为真,0为假
	DWORD MinBtn;		//最小化按钮,1为真,0为假
	DWORD CtrlBtn;		//控制按钮,1为真,0为假
	DWORD pos;			//位置,0为通常,1为居中,2为最小化,3为最大化
	DWORD CanMove;		//可否移动,1为真,0为假
	DWORD PlayCount;	//播放次数,0为循环播放,1为仅播放1次,2为不播放
	DWORD EnterToNext;	//回车下移焦点,1为真,0为假
	DWORD EscClose;		//Esc键关闭,1为真,0为假
	DWORD F1OpenHelp;	//F1键打开帮助,1为真,0为假
	DWORD HelpContext;	//帮助标志值
	DWORD ShowInTaskBar; //在任务条中显示,1为真,0为假
	DWORD HitMove;		//随意移动,1为真,0为假
	DWORD shape;		//外形,取值范围为0-29
	DWORD Topmost;		//总在最前,1为真,0为假
	DWORD KeepTitleBarActive;//保持标题条激活,1为真,0为假

	DWORD ClassNameSize; //窗口类名长度
	//BYTE ClassName[ClassNameSize];

	DWORD BackPicSize;   //底图大小,0表示无底图
	//BYTE BackPic[BackPicSize];

	DWORD iconSize;		 //图标大小
	//BYTE icon[iconSize];

	DWORD BackMusicSize; //背景音乐大小                   
	//BYTE BackMusic[BackMusicSize];

	DWORD captionSize;   //标题长度                        
	//BYTE caption[captionSize];

	DWORD HelpFileNameSize;  //帮助文件名长度
	//BYTE HelpFileName[HelpFileNameSize];
}*PAnaly_WINDOW;

typedef struct Analy_EDIT
{
	DWORD ChildWindowId;	//控件类型
	BYTE ZeroByte[0x14];	
	string CtrlName;		//控件名称
	BYTE UnknowByte1[0x5];
	DWORD left;				//左边
	DWORD top;				//顶边
	DWORD width;			//宽度
	DWORD height;			//高度
	BYTE UnknowByte2[0xC];
	DWORD MousePointerSize;	//自定义鼠标指针所占字节集的大小,如果是选项中的鼠标指针,则为4
	DWORD MousePointer;
	//BYTE customMousePointer[MousePointerSize]; //自定义鼠标指针字节集,如果为选项中的鼠标指针,则此值不存在
							//选项中的鼠标指针特殊值,如果为自定义鼠标指针,则为-1,
							//默认型为0
							//标准箭头型 0x7F00,十字型0x7F03
							//文本编辑型 0x7F01,沙漏型0x7F02
							//箭头及问号型 0x7F8B,箭头及沙漏型0x7F8A
							//禁止符型0x7F88,四向箭头型0x7F86
							//北<->东箭头型0x7F83,北<->南箭头型0x7F85
							//北<->西箭头型0x7F82,西<->东箭头型0x7F84
							//向上箭头型0x7F04,手型0x7F89

	string tag;				//标记
	DWORD ZeroByte1;
	DWORD vis_dis_stopfoc;	//二进制第一位为可视,1为真,0为假;第二位为禁止,1为真,0为假
							//第二位为可停留焦点,1为真,0为假
	DWORD Stoprder;			//停留顺序
	DWORD UnknowSize;		//下面这个字段的长度
	BYTE UnknowByte3[0x20];	//尚不清楚，而且长度有时候不一样。
	DWORD borde;			//边框
							//0为无边框
							//1为凹入式
							//2为凸出式
							//3为凹入式
							//4为镜框式
							//5为单边边框式
	DWORD TextColor;		//文本颜色
	DWORD BackColor;		//背景颜色
	DWORD HideSelect;		//隐藏选择
	DWORD MaxSize;			//最大允许长度
	DWORD IsMoreLine;		//是否允许多行
	DWORD ScrollBar;		//滚动条
							//0为无
							//1为横向滚动条
							//2为纵向滚动条
							//3为横线及纵向
	DWORD Alignment;		//对齐方式
							//0为左对齐
							//1为居中
							//2为右对齐
	DWORD ImportMode;		//输入模式
	WORD PassCoverChar;		//密码覆盖字符
	DWORD transition;		//转换方式
							//0为无
							//1为大写 -> 小写
							//2为小写 -> 大写
	DWORD Regulator;		//调节器方式，
							//0为无调节器
							//1为自动调节器
							//2为手动调节器
	DWORD RegulatorMin;		//调节器底限阈值
	DWORD RegulatorMax;		//调节器上限阈值
	DWORD StartSelPos;		//起始选择位置
	DWORD BeSelCharNum;		//被选择字节数


	//
	// 字体结构有点不懂。。。。。
	//
	DWORD FontSize;			//字体信息长度
	//BYTE FontInfo[FontSize];

	DWORD ContentSize;		//内容长度
	//BYTE Content[ContentSize];
}*PAnaly_EDIT;
#pragma pack(pop)
// CDialogWin 对话框

class CDialogWin : public CDialog
{
	DECLARE_DYNAMIC(CDialogWin)

public:
	CDialogWin(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogWin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();


	void TreeShowInfo();
	void TreeSetIco(DWORD CtrlType, HTREEITEM node);

public:
	CImageList m_ico;
	CTreeCtrl m_Tree_win;
	CStatic m_Static_Name;
	CListCtrl m_CtrlInfo;

	HTREEITEM m_root;						//m_Tree_win 的根
	
	vector<MEMTEMP> m_MemTemp;				//临时内存信息

	EWINDOWINFO m_WindowInfo;				//窗口信息
	vector<ECONTROLLIST> m_Control_List;	//所以控件的信息
	
	map<DWORD, string> m_MousePointType;	// 鼠标指针类型
	vector<string> m_borderType;			// 边框类型
	vector<string> m_ImportMode;			// 输入模式
	afx_msg void CDialogWin::OnClose();
	afx_msg void OnSelchangingTreeWin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListCtrlinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawListCtrlinfo(NMHDR* pNMHDR, LRESULT* pResult);
public:
	//
	//每个控件一个解析函数
	//

	//
	// 窗口
	//
	void Analy_Window(BYTE* CtrlInf, DWORD ItemData);
	//
	// 编辑框
	//
	void Analy_Edit(BYTE* CtrlInf, DWORD ItemData);
	
	afx_msg void OnSaveFile();
};
