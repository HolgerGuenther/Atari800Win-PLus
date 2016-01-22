/****************************************************************************
File    : SettingsDlg.cpp
/*
@(#) #SY# Atari800Win PLus
@(#) #IS# CSettingsDlg implementation file
@(#) #BY# Tomasz Szymankowski
@(#) #LM# 27.09.2002
*/

#include "StdAfx.h"
#include "Atari800Win.h"
#include "Helpers.h"
#include "SettingsAdvDlg.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_SETTINGS_FIRST		IDC_SETTINGS_DISABLEBASIC
#define IDC_SETTINGS_LAST		IDC_SETTINGS_CANCEL


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

BEGIN_MESSAGE_MAP(CSettingsDlg, CCommonDlg)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_SETTINGS_DISABLEBASIC, OnDisableBasic)
	ON_BN_CLICKED(IDC_SETTINGS_ENABLESIO, OnEnableSio)
	ON_BN_CLICKED(IDC_SETTINGS_ENABLEH, OnEnableH)
	ON_BN_CLICKED(IDC_SETTINGS_ENABLEP, OnEnableP)
	ON_BN_CLICKED(IDC_SETTINGS_ENABLER, OnEnableR)
	ON_BN_CLICKED(IDC_SETTINGS_HARDREADONLY, OnHardReadOnly)
	ON_CBN_SELCHANGE(IDC_SETTINGS_PRINTMETHOD, OnSelchangePrintMethod)
	ON_EN_KILLFOCUS(IDC_SETTINGS_PRINTPARAMS, OnKillfocusPrintParams)
	ON_BN_CLICKED(IDC_SETTINGS_ADVSETTING, OnAdvSetting)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETTINGS_OK, OnOK)
	ON_BN_CLICKED(IDC_SETTINGS_CANCEL, CCommonDlg::OnCancel)
END_MESSAGE_MAP()

/*========================================================
Method   : CSettingsDlg::CSettingsDlg
=========================================================*/
/* #FN#
   Standard constructor */
CSettingsDlg::
CSettingsDlg(
	CWnd *pParent /*=NULL*/ /* #IN# Pointer to the parent window */
)
	: CCommonDlg( CSettingsDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT(CSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bReboot    = FALSE;
	m_nFirstCtrl = IDC_SETTINGS_FIRST;
	m_nLastCtrl  = IDC_SETTINGS_LAST;

} /* #OF# CSettingsDlg::CSettingsDlg */

/*========================================================
Method   : CSettingsDlg::DoDataExchange
=========================================================*/
/* #FN#
   Dynamic Data Exchange (not used) */
void
/* #AS#
   Nothing */
CSettingsDlg::
DoDataExchange(
	CDataExchange *pDX /* #IN# Pointer to CDataExchange object */
)
{
	CCommonDlg::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CSettingsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

} /* #OF# CSettingsDlg::DoDataExchange */


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg implementation

/*========================================================
Method   : CSettingsDlg::SetDlgState
=========================================================*/
/* #FN#
   Sets up the state of the dialog controls */
void
/* #AS#
   Nothing */
CSettingsDlg::
SetDlgState()
{
	/* Set up check buttons states */
	_SetChkBttn( IDC_SETTINGS_DISABLEBASIC, (BOOL)m_nDisableBasic );
	_SetChkBttn( IDC_SETTINGS_ENABLESIO,    (BOOL)m_nEnableSIOPatch );
	_SetChkBttn( IDC_SETTINGS_ENABLEH,      (BOOL)m_nEnableHPatch );
	_SetChkBttn( IDC_SETTINGS_ENABLEP,      (BOOL)m_nEnablePPatch );
	_SetChkBttn( IDC_SETTINGS_ENABLER,      (BOOL)m_nEnableRPatch );
	_SetChkBttn( IDC_SETTINGS_HARDREADONLY, (BOOL)m_nHardReadOnly );

	/* Alternative Print Methods */
	_SetSelCbox( IDC_SETTINGS_PRINTMETHOD, m_nPrintMethod + 1 );
    if (m_nPrintMethod >= 0 && m_nPrintMethod < PRINT_METHOD_COUNT)
	    SetDlgItemText( IDC_SETTINGS_PRINTPARAMS, m_szPrintParams[m_nPrintMethod] );
    else
        SetDlgItemText( IDC_SETTINGS_PRINTPARAMS, "(default print action)" );

#ifdef WIN_NETWORK_GAMES
	if( ST_KAILLERA_ACTIVE )
	{
		_EnableCtrl( IDC_SETTINGS_DISABLEBASIC, FALSE );
		_EnableCtrl( IDC_SETTINGS_ENABLESIO,    FALSE );
		_EnableCtrl( IDC_SETTINGS_ENABLEH,      FALSE );
		_EnableCtrl( IDC_SETTINGS_ENABLEP,      FALSE );
		_EnableCtrl( IDC_SETTINGS_ENABLER,      FALSE );
		_EnableCtrl( IDC_SETTINGS_HARDREADONLY, FALSE );
		_EnableCtrl( IDC_SETTINGS_PRINTMETHOD,  FALSE );
		_RdOnlyEdit( IDC_SETTINGS_PRINTPARAMS,  TRUE  );
	}
	else
#endif
	{
		_EnableCtrl( IDC_SETTINGS_ENABLER,      RDevice_IsCapable() != -1 );
		_EnableCtrl( IDC_SETTINGS_HARDREADONLY, (BOOL)m_nEnableHPatch );
		_EnableCtrl( IDC_SETTINGS_PRINTMETHOD,  (BOOL)m_nEnablePPatch );
		_EnableCtrl( IDC_SETTINGS_PRINTPARAMS,  m_nEnablePPatch && (m_nPrintMethod >= 0) );
	}
} /* #OF# CSettingsDlg::SetDlgState */


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

/*========================================================
Method   : CSettingsDlg::OnInitDialog
=========================================================*/
/* #FN#
   Performs special processing when the dialog box is initialized */
BOOL
/* #AS#
   TRUE unless you set the focus to a control */
CSettingsDlg::
OnInitDialog()
{
	CCommonDlg::OnInitDialog();

	m_ulMiscState     = g_Misc.ulState;
	m_nDisableBasic   = Atari800_disable_basic;
	m_nEnableSIOPatch = ESC_enable_sio_patch;
	m_nEnableHPatch   = Devices_enable_h_patch;
	m_nEnablePPatch   = Devices_enable_p_patch;
	m_nEnableRPatch   = Devices_enable_r_patch;
	m_nHardReadOnly   = Devices_h_read_only;
	m_nEnableRTime    = RTIME_enabled;

    m_nPrintMethod = Devices_print_method;
    for (int i = 0; i < PRINT_METHOD_COUNT; i++)
	    strcpy( m_szPrintParams[i], Devices_print_params[i] );

	SetDlgState();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
} /* #OF# CSettingsDlg::OnInitDialog */

/*========================================================
Method   : CSettingsDlg::OnDisableBasic
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnDisableBasic()
{
	m_nDisableBasic = _GetChkBttn( IDC_SETTINGS_DISABLEBASIC );

} /* #OF# CSettingsDlg::OnDisableBasic */

/*========================================================
Method   : CSettingsDlg::OnEnableSio
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnEnableSio() 
{
	m_nEnableSIOPatch = _GetChkBttn( IDC_SETTINGS_ENABLESIO );

} /* #OF# CSettingsDlg::OnEnableSio */

/*========================================================
Method   : CSettingsDlg::OnEnableH
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnEnableH()
{
	m_nEnableHPatch = _GetChkBttn( IDC_SETTINGS_ENABLEH );
	SetDlgState();

} /* #OF# CSettingsDlg::OnEnableH */

/*========================================================
Method   : CSettingsDlg::OnEnableP
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnEnableP()
{
	m_nEnablePPatch = _GetChkBttn( IDC_SETTINGS_ENABLEP );
	SetDlgState();

} /* #OF# CSettingsDlg::OnEnableP */

/*========================================================
Method   : CSettingsDlg::OnEnableR
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnEnableR()
{
	m_nEnableRPatch = _GetChkBttn( IDC_SETTINGS_ENABLER );

} /* #OF# CSettingsDlg::OnEnableR */

/*========================================================
Method   : CSettingsDlg::OnHardReadOnly
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate check box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnHardReadOnly()
{
	m_nHardReadOnly = _GetChkBttn( IDC_SETTINGS_HARDREADONLY );

} /* #OF# CSettingsDlg::OnHardReadOnly */

/*========================================================
Method   : CSettingsDlg::OnSelchangePrintMethod
=========================================================*/
/* #FN#
   Sets a state of the object regarding to an appropriate combo box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnSelchangePrintMethod()
{
    int itemindex;
	if( CB_ERR == (itemindex = _GetSelCbox( IDC_SETTINGS_PRINTMETHOD )) )
	{
		_SetSelCbox( IDC_SETTINGS_PRINTMETHOD, 0 ),
		m_nPrintMethod = PRINT_METHOD_DEFAULT;
	}
    else
    {
        m_nPrintMethod = itemindex - 1;
    }
	SetDlgState();

} /* #OF# CSettingsDlg::OnSelchangePrintMethod */

/*========================================================
Method   : CSettingsDlg::OnKillfocusPrintParams
=========================================================*/
/* #FN#
   The framework calls this function before an edit losing input focus */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnKillfocusPrintParams()
{
    if (m_nPrintMethod >= 0 && m_nPrintMethod < PRINT_METHOD_COUNT)
	GetDlgItemText( IDC_SETTINGS_PRINTPARAMS, m_szPrintParams[m_nPrintMethod], PRINT_PARAMS_LENGTH );

} /* #OF# CSettingsDlg::OnKillfocusPrintParams */

/*========================================================
Method   : CSettingsDlg::OnAdvSetting
=========================================================*/
/* #FN#
   Displays "Advanced Atari Setting" dialog box */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnAdvSetting()
{
	CSettingsAdvDlg dlgSettingsAdv( &m_ulMiscState,
									&m_nEnableRTime,
									this );
	dlgSettingsAdv.DoModal();

} /* #OF# CSettingsDlg::OnAdvSetting */

/*========================================================
Method   : CSettingsDlg::ReceiveFocused
=========================================================*/
/* #FN#
   Receives the edit controls content again. The user could press
   'Enter' or 'Alt-O' and then all changes he's made in the last
   edited control would be lost. */
void
/* #AS#
   Nothing */
CSettingsDlg::
ReceiveFocused()
{
	CWnd *pWnd    = GetFocus();
	UINT  nCtrlID = pWnd ? pWnd->GetDlgCtrlID() : 0;

	if( IDC_SETTINGS_PRINTPARAMS == nCtrlID )
	{
		OnKillfocusPrintParams();
	}
} /* #OF# CSettingsDlg::ReceiveFocused */

/*========================================================
Method   : CSettingsDlg::OnOK
=========================================================*/
/* #FN#
   Called when the user clicks the OK button */
void
/* #AS#
   Nothing */
CSettingsDlg::
OnOK() 
{
	BOOL bCommit = TRUE;

	/* Unfortunately, edit controls do not lose the focus before
	   handling this when the user uses accelerators */
	ReceiveFocused();

	/* Check if the video recording should be stopped */
	if( m_nEnableRTime != RTIME_enabled )
	{
		bCommit = StreamWarning( IDS_WARN_RECORD_SYSTEM, SRW_VIDEO_STREAM );
	}
	if( bCommit )
	{
		if( m_nDisableBasic != Atari800_disable_basic )
		{
			Atari800_disable_basic = m_nDisableBasic;
			WriteRegDWORD( NULL, REG_DISABLE_BASIC, Atari800_disable_basic );

	//		m_bReboot = TRUE; /* I think we don't really need this */
		}
		if( m_nEnableSIOPatch != ESC_enable_sio_patch )
		{
			ESC_enable_sio_patch = m_nEnableSIOPatch;
			WriteRegDWORD( NULL, REG_ENABLE_SIO_PATCH, ESC_enable_sio_patch );

			ESC_UpdatePatches();
		}
		if( m_nEnableHPatch != Devices_enable_h_patch )
		{
			Devices_enable_h_patch = m_nEnableHPatch;
			WriteRegDWORD( NULL, REG_ENABLE_H_PATCH, Devices_enable_h_patch );

			ESC_UpdatePatches();
		}
		if( m_nEnablePPatch != Devices_enable_p_patch )
		{
			Devices_enable_p_patch = m_nEnablePPatch;
			WriteRegDWORD( NULL, REG_ENABLE_P_PATCH, Devices_enable_p_patch );

			ESC_UpdatePatches();
		}
		if( m_nEnableRPatch != Devices_enable_r_patch )
		{
			Devices_enable_r_patch = m_nEnableRPatch;
			WriteRegDWORD( NULL, REG_ENABLE_R_PATCH, Devices_enable_r_patch );

			RDevice_UpdatePatches();
			ESC_UpdatePatches();
		}
		if( m_nHardReadOnly != Devices_h_read_only )
		{
			Devices_h_read_only = m_nHardReadOnly;
			WriteRegDWORD( NULL, REG_HD_READ_ONLY, Devices_h_read_only );
		}
		if( m_nEnableRTime != RTIME_enabled )
		{
			RTIME_enabled = m_nEnableRTime;
			WriteRegDWORD( NULL, REG_ENABLE_RTIME, RTIME_enabled );

			m_bReboot = TRUE;
		}
		if( m_ulMiscState != g_Misc.ulState )
		{
			if( !_IsFlagSet( g_Misc.ulState, MS_NO_ATTRACT_MODE ) &&
				 _IsFlagSet( m_ulMiscState, MS_NO_ATTRACT_MODE ) )
			{
				g_Misc.nAttractCounter = ATTRACT_CHECK_FRAMES;
			}
			g_Misc.ulState = m_ulMiscState;
			WriteRegDWORD( NULL, REG_MISC_STATE, g_Misc.ulState );
		}

		if( m_nPrintMethod != Devices_print_method )
		{
			Devices_print_method = m_nPrintMethod;
			WriteRegDWORD( NULL, REG_PRINT_METHOD, Devices_print_method );
		}

        for (int i = 0; i < PRINT_METHOD_COUNT; i++)
        {
 		    if( *m_szPrintParams[i] == '\0' )
			    strcpy( m_szPrintParams[i], DEF_PRINT_COMMAND );
		    if( _stricmp( m_szPrintParams[i], Devices_print_params[i] ) != 0 )
		    {
			    strcpy( Devices_print_params[i], m_szPrintParams[i]);
                char key[32];
                sprintf (key, REG_PRINT_PARAMS_FMT, i);
			    WriteRegString (NULL, key, Devices_print_params[i]);
		    }
        }
		CCommonDlg::OnOK();
	}
} /* #OF# CSettingsDlg::OnOK */
