/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 
日期: 2014-05-09
功能: 钢包等级实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
//#include "ttmsmd1.h"


/*<remark>=========================================================
/// <summary>
/// 钢包等级实绩修改
/// <para>
/// 1.接收传入的钢包等级实绩信息；
/// 2.修改钢包等级实绩信息；
/// </para>
/// <para>数据库表：TTMSMD1(钢包等级实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面使用实绩修改调用；TTMSMD1画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的钢包等级实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsmd1_upd)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsmd1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	CString v_group = "";
	CString v_shift = "";
	CString v_div = "";
	
	/* 实体类定义 */
	//CTTMSMD1 ttmsmd1(conn);
	CModel ttmsm01("TTMSM01");
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		////记录总条数
		//int rows = bcls_rec->Tables[0].Rows.get_Count();
		//v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		//Log::Trace("","tmsmd1_upd","v_div [{0}]" ,(const char*)v_div);

		//if(v_div.Trim() =="TMSM04")
		//{
		//	//循环处理
		//	for (int i = 0; i < rows ; i++ )
		//	{
		//		// 取得单行传入信息 
		//		ttmsmd1.Reset();
		//		ttmsmd1.MergeFrom(bcls_rec->Tables[0].Rows[i]);
		//		
		//		Log::Trace("","tmsmd1_upd","ttmsmd1.LADEL_LEVEL [{0}]"		,(const char*)ttmsmd1.LADEL_LEVEL);
		//		Log::Trace("","tmsmd1_upd","ttmsmd1.SM_UNIT_NO [{0}]"	,(const char*)ttmsmd1.SM_UNIT_NO);
		//		Log::Trace("","tmsmd1_upd","ttmsmd1.LADLE_NO [{0}]"	,(const char*)ttmsmd1.LADLE_NO);

		//		//班次班组 shift_no shift_group
		//		f_epep_get_shift_group("SM",ttmsmd1.REC_CREATE_TIME,v_shift,v_group,conn );

		//		// 判传入的参数是否有错	
		//	
		//		if(ttmsmd1.SM_UNIT_NO.Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//		}
		//		if(ttmsmd1.LADLE_NO.Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的钢包号不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//		}
		//		if(ttmsmd1.LADEL_LEVEL.Trim() == "")
		//		{
		//			sprintf	(s.msg , "传入的钢包钢包等级不能为空") ;
		//			throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//		}
		//		
		//		/*查询钢包使用实绩表*/
		//		sqlstr = "  SELECT COUNT(1) "
		//				 "  FROM TTMSMD1 "
		//				 "  WHERE LADEL_LEVEL		= @ttmsmd1.LADEL_LEVEL ";
		//				 
		//		cmd_inq.SetCommandText( sqlstr );
		//		cmd_inq.Parameters.Set( "ttmsmd1.LADEL_LEVEL"		, ttmsmd1.LADEL_LEVEL );
		//		cmd_inq.ExecuteReader();
		//		count = 0 ;
		//		if(cmd_inq.Read())
		//		{
		//			count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
		//		}
		//		Log::Trace("","tmsmd1_upd","count [{0}]",count );
		//		cmd_inq.Close();
		//		if	(count == 0)
		//		{
		//			sprintf	(s.msg ,"钢包等级实绩信息不存在");
		//			/*新增钢包使用实绩信息报错信息档*/
		//			throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//		}
		//		
		//		//设置修改者信息
		//		ttmsmd1.REC_REVISOR = s.userid;
		//		ttmsmd1.REC_REVISE_TIME = datetimeNow;

		//		//根据传入信息更新钢包使用实绩表
		//		sqlstr = "UPDATE TTMSMD1"; //用于捕获数据库操作异常情况
		//		ttmsmd1.Update(	" REC_REVISOR "
		//						" ,REC_REVISE_TIME "
		//						" ,ARCHIVE_FLAG "
		//						" ,LADEL_LEVEL "
		//						" ,PAUSE_TIME "
		//						" ,NOTE "
		//						" ,LADEL_COLOUR "
		//						" ,SM_UNIT_NO "
		//						" ,LADLE_NO ");
		//	}
		//}
		//else if(v_div.Trim() == "TMSM01")
		//{
		//	/*重置TTMSM05*/
		//	ttmsm01.Reset();
		//	/*读取传入的参数*/
		//	ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//	Log::Trace("","tmsmd1_upd","ttmsm01.LADLE_NO [{0}]"		,(const char*)ttmsm01["LADLE_NO"].ToString());
		//	Log::Trace("","tmsmd1_upd","ttmsm01.SM_UNIT_NO [{0}]"	,(const char*)ttmsm01["SM_UNIT_NO"].ToString());

		//	// 判传入的参数是否有错			
		//	if(ttmsm01["SM_UNIT_NO"].ToString().Trim() == "")
		//	{
		//		sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
		//		throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//	}
		//	if(ttmsm01["LADLE_NO"].ToString().Trim() == "")
		//	{
		//		sprintf	(s.msg , "传入的钢包号不能为空") ;
		//		throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//	}

		//	//班次班组 shift_no shift_group
		//	f_epep_get_shift_group("SM",ttmsm01["REC_CREATE_TIME"].ToString(),v_shift,v_group,conn );

		//	/*查询钢包使用实绩表*/
		//	sqlstr = "  SELECT COUNT(1) "
		//			 "  FROM TTMSMD1 "
		//			 "  WHERE LADEL_LEVEL		= @ttmsmd1.LADEL_LEVEL ";
		//			 
		//	cmd_inq.SetCommandText( sqlstr );
		//	cmd_inq.Parameters.Set( "ttmsmd1.LADEL_LEVEL"		, ttmsm01.LADEL_LEVEL );
		//	cmd_inq.ExecuteReader();
		//	count = 0 ;
		//	if	(cmd_inq.Read())
		//	{
		//		count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
		//		Log::Trace("","tmsmd1_upd","count [{0}]",count );
		//	}
		//	cmd_inq.Close();
		//	if(count == 0)
		//	{
		//		sprintf	(s.msg ,"钢包等级实绩信息不存在");
		//		/*新增钢包使用实绩信息报错信息档*/
		//		throw	CApplicationException(-1 , s.msg , "tmsmd1_upd");
		//	}
		//	
		//	/*修改钢包使用实绩表*/
		//	ttmsmd1.CopyFrom(ttmsm01);
		//	//ttmsmd1.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
		//	//ttmsmd1.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
		//	ttmsmd1.REC_REVISOR = s.userid;
		//	ttmsmd1.REC_REVISE_TIME = datetimeNow;
		//	//班次班组 shift_no shift_group

		//	sqlstr = "UPDATE TTMSMD1"; //用于捕获数据库操作异常情况
		//	ttmsmd1.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,ARCHIVE_FLAG "
		//					" ,LADEL_LEVEL "
		//					" ,PAUSE_TIME "
		//					" ,NOTE "
		//					" ,LADEL_COLOUR"
		//					" ,SM_UNIT_NO,LADLE_NO ");

		//	sqlstr = "UPDATE TTMSM01";
		//	ttmsm01.Update(	" REC_REVISOR "
		//					" ,REC_REVISE_TIME "
		//					" ,LADEL_LEVEL"
		//					" SM_UNIT_NO,LADLE_NO ");			
		//}
		////strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		//strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
