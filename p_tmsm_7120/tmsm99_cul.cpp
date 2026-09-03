/*****************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2011
Author:      T01966
Version:     3.0
Date:        2014-01-17 13:15:20
Description: TMSM99 铁水包包龄计算
**************************************************/

// 框架头文件
#include "stdafx.h"

// 业务头文件

//#include "ttmsm99.h"

/*<remark>=========================================================
/// <summary>
/// 铁水包包龄计算
/// <para>
/// 1.0 根据输入参数，计算铁水包包龄；
/// </para>
/// <para>数据库表：TTMSM99(铁包包龄计算参数表)；</para>
/// <para>主调用函数：前台TMSM11画面查询调用。</para>
/// </summary>
/// <param name=""></param>
/// <returns>铁包信息</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(tmsm99_cul)

int f_tmsm99_cul(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */	
	int doFlag = 0;		//返回值

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);

    /* 实体类定义 */
	CModel ttmsm11("TTMSM11");
	//CTTMSM99 ttmsm99(conn);

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";

	/* 业务变量 */
	CString v_sm_unit_no  = "";	//炼钢单元号
	CDecimal v_ladle_life = 0;

	bcls_ret->Tables[0].set_TableName("TTMSM11");
	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL,"IRON_LADLE_LIFE");

	try 
	{
		////接收从前台传入的值
		//ttmsm11.Reset();
		//ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);	

		//Log::Trace(" ", "tmsm99_cul", "ttmsm11.MOLTIRON_WT[{0}]"	,ttmsm11["MOLTIRON_WT"].ToDecimal());
		//Log::Trace(" ", "tmsm99_cul", "ttmsm11.MODEL_ID[{0}]"		,(const char*)ttmsm11.MODEL_ID);
		//Log::Trace(" ", "tmsm99_cul", "ttmsm11.IS_STANDARD_JAR[{0}]",(const char*)ttmsm11.IS_STANDARD_JAR);
		//Log::Trace(" ", "tmsm99_cul", "ttmsm11.IS_IRON_LTREAT[{0}]"	,(const char*)ttmsm11.IS_IRON_LTREAT);
		//
		//if(ttmsm11.MODEL_ID.Trim() == "")
		//{
		//	sprintf(s.msg,"公式号不能为空！");
		//	throw CApplicationException(-1,s.msg,"tmsm99_cul");
		//}
		////建立查询数据SQL语句查询铁包使用实绩表
		//sqlstr =	" SELECT * "
		//			" FROM   TTMSM99 "
		//			" WHERE  MODEL_ID = @ttmsm11.MODEL_ID ";

		////连接数据库
		//cmd.SetCommandText( sqlstr );

		////传递参数进SQL语句
		//cmd.Parameters.Set("ttmsm11.MODEL_ID"	, ttmsm11.MODEL_ID);
		//cmd.ExecuteReader();
		//ttmsm99.Reset();
		//if (cmd.Read())
		//{
		//	cmd.Fetch(ttmsm99);//把数据都压在头文件里面
		//}
		//cmd.Close();

		//ttmsm99.Print();

		//v_ladle_life = 0;

		//// 倒罐
		//if(ttmsm11.IS_STANDARD_JAR.Trim() == "0" || ttmsm11.IS_STANDARD_JAR.Trim() == "")
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.TPD_STD_PARA;
		//}
		//else if(ttmsm11.IS_STANDARD_JAR.Trim() == "1")
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.TPD_NSTD_PARA;
		//}
		//Log::Trace(" ", "tmsm99_cul", "倒罐：v_ladle_life[{0}]"	,v_ladle_life);

		//// 来水
		//if(ttmsm11["MOLTIRON_WT"].ToDecimal() <= ttmsm99.MOLTIRON_WT_A)
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.MOLTIRON_PARA_X;
		//}
		//else if(ttmsm99.MOLTIRON_WT_A < ttmsm11["MOLTIRON_WT"].ToDecimal() < ttmsm99.MOLTIRON_WT_B)
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.MOLTIRON_PARA_Y;
		//}
		//else if(ttmsm11["MOLTIRON_WT"].ToDecimal() >= ttmsm99.MOLTIRON_WT_B)
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.MOLTIRON_PARA_Z;
		//}
		//Log::Trace(" ", "tmsm99_cul", "来水：v_ladle_life[{0}]"	,v_ladle_life);
		//// 脱硫
		//if(ttmsm11.IS_IRON_LTREAT.Trim() == "0" || ttmsm11.IS_IRON_LTREAT.Trim() == "")
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.NS_PARA;
		//}
		//else if(ttmsm11.IS_IRON_LTREAT.Trim() == "1")
		//{
		//	v_ladle_life = v_ladle_life + ttmsm99.S_PARA;
		//}
		//Log::Trace(" ", "tmsm99_cul", "脱硫：v_ladle_life[{0}]"	,v_ladle_life);

		//bcls_ret->Tables[0].Rows.Add();
		//bcls_ret->Tables[0].Rows[0]["IRON_LADLE_LIFE"] = v_ladle_life;  

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
