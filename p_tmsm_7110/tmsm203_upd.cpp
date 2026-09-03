/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-08-13
Description:上传图片导入
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件
//外部函数声明
BM2F_ENTERACE(tmsm203_upd)

int f_tmsm203_upd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */
	
	/* 实体类定义 */
	CDbCommand cmd_inq(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_count = 0;

	try
	{
		CString v_table_name = bcls_rec->Tables[1].Rows[0]["TABLENAME"].ToString();
		CModel ttmsmyl(v_table_name);
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			ttmsmyl.Reset();
			ttmsmyl.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			ttmsmyl["C_UPLOADSIGN"] = "1";
			ttmsmyl["DATE_C"] = ttmsmyl["DATE_C"].ToString().SubstringNE(0, 8);
			
			ttmsmyl.Update("C_UPLOADSIGN", "DATE_C,SEQ_NO");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


