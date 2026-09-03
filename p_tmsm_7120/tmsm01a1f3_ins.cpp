/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-01
Description: 钢包信息新增
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息新增
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(tmsm01a1f3_ins)                                         

int f_tmsm01a1f3_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	
	/* 实体类定义 */ 
	CModel ttmsm01("TTMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm01.Reset();
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm01.TrimOrBlank();

			ttmsm01["SM_UNIT_NO"]      = "A";

			Log::Trace("",__FUNCTION__,"ttmsm01.SM_UNIT_NO		= [{0}]",(const char*)ttmsm01["SM_UNIT_NO"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm01.LADLE_NO		= [{0}]",(const char*)ttmsm01["LADLE_NO"].ToString());

			/* 检查输入参数合法性 */
			if(ttmsm01["LADLE_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"钢包号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该钢包号是否存在 */
			if(ttmsm01.QueryCount("SM_UNIT_NO,LADLE_NO") > 0)
			{
				sprintf(s.msg,"钢包号[%s]已存在",(const char*)ttmsm01["LADLE_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 设置钢包信息表初始值 */
			//ttmsm01["LADLE_LIFE"]		= 0;			//包龄
			//ttmsm01.MAT_LINE_TYPE		= "BW";			//产线类型
			//ttmsm01.MAT_KIND			= "SM";			//物料种类

	 
			ttmsm01["REC_CREATOR"]			= s.userid;		//记录创建责任者
			ttmsm01["REC_CREATE_TIME"]		= datetime;		//记录创建时刻
			ttmsm01.TrimOrBlank();
			ttmsm01.Insert();
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
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
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

