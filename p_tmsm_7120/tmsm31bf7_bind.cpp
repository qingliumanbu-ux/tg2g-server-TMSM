/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车作业命令新增
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件

//外部函数声明


BM2F_ENTERACE(tmsm31bf7_bind)

int f_tmsm31bf7_bind(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	

	/* 业务变量 */
	CString	datetime("");
	CString copper1("");//铜板位置1
	CString copper2("");//铜板位置2
	CString copper3("");//铜板位置3
	CString copper4("");//铜板位置4
	/* 实体类定义 */
	CModel ttmsm31("TTMSM31");//结晶器信息表
	CModel ttmsm41("TTMSM41");//铜板信息表
	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("COPPER1"))
			{
				copper1 = bcls_rec->Tables[0].Rows[i]["COPPER1"].ToString().Trim();
			}
			if (bcls_rec->Tables[0].Columns.Contains("COPPER2"))
			{
				copper2 = bcls_rec->Tables[0].Rows[i]["COPPER2"].ToString().Trim();
			} 
			if (bcls_rec->Tables[0].Columns.Contains("COPPER3"))
			{
				copper3 = bcls_rec->Tables[0].Rows[i]["COPPER3"].ToString().Trim();
			}
			if (bcls_rec->Tables[0].Columns.Contains("COPPER4"))
			{
				copper4 = bcls_rec->Tables[0].Rows[i]["COPPER4"].ToString().Trim();
			}
			ttmsm31.Reset();
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm31.TrimOrBlank();

			ttmsm31["REC_REVISOR"] = s.userid;		//记录修改责任者
			ttmsm31["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			ttmsm31.TrimOrBlank();
			ttmsm31.Print();

			ttmsm31.Update("REC_REVISOR,REC_REVISE_TIME,COPPER1,COPPER2,COPPER3,COPPER4","MOLD_NO");

			/* 查询该钢包号是否存在 */
			//if (ttmsm31.QueryCount("MOLD_NO") <= 0)
			//{
			//	sprintf(s.msg, "行车命令号[%s]不存在", (const char*)ttmsm53["CRANE_INST_NO"].ToString());
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}

			ttmsm41.Reset();
			ttmsm41["MD_COPPER_PLATE_NO"] = copper1;
			if (ttmsm41.QueryCount("MD_COPPER_PLATE_NO")>0)
			{

				ttmsm41["MOLD_NO"] = ttmsm31["MOLD_NO"];
				ttmsm41["CURRENT_STATUS"] = "1";
				ttmsm41["TM_INSTALLATION_LOCATION"] = "东侧";
				ttmsm41.Update("REC_REVISOR,REC_REVISE_TIME,CURRENT_STATUS,TM_INSTALLATION_LOCATION,MOLD_NO","MD_COPPER_PLATE_NO");
			}


			ttmsm41.Reset();
			ttmsm41["MD_COPPER_PLATE_NO"] = copper2;
			if (ttmsm41.QueryCount("MD_COPPER_PLATE_NO") > 0)
			{
				ttmsm41["REC_REVISOR"] = s.userid;		//记录修改责任者
				ttmsm41["REC_REVISE_TIME"] = datetime;		//记录修改时刻
				ttmsm41["MOLD_NO"] = ttmsm31["MOLD_NO"];
				ttmsm41["CURRENT_STATUS"] = "1";
				ttmsm41["TM_INSTALLATION_LOCATION"] = "西侧";
				ttmsm41.Update("REC_REVISOR,REC_REVISE_TIME,CURRENT_STATUS,TM_INSTALLATION_LOCATION,MOLD_NO", "MD_COPPER_PLATE_NO");
			}

			ttmsm41.Reset();
			ttmsm41["MD_COPPER_PLATE_NO"] = copper3;
			if (ttmsm41.QueryCount("MD_COPPER_PLATE_NO") > 0)
			{
				ttmsm41["REC_REVISOR"] = s.userid;		//记录修改责任者
				ttmsm41["REC_REVISE_TIME"] = datetime;		//记录修改时刻
				ttmsm41["MOLD_NO"] = ttmsm31["MOLD_NO"];
				ttmsm41["CURRENT_STATUS"] = "1";
				ttmsm41["TM_INSTALLATION_LOCATION"] = "南侧";
				ttmsm41.Update("REC_REVISOR,REC_REVISE_TIME,CURRENT_STATUS,TM_INSTALLATION_LOCATION,MOLD_NO", "MD_COPPER_PLATE_NO");
			}
			ttmsm41.Reset();
			ttmsm41["MD_COPPER_PLATE_NO"] = copper4;
			if (ttmsm41.QueryCount("MD_COPPER_PLATE_NO") > 0)
			{
				ttmsm41["REC_REVISOR"] = s.userid;		//记录修改责任者
				ttmsm41["REC_REVISE_TIME"] = datetime;		//记录修改时刻
				ttmsm41["MOLD_NO"] = ttmsm31["MOLD_NO"];
				ttmsm41["CURRENT_STATUS"] = "1";
				ttmsm41["TM_INSTALLATION_LOCATION"] = "北侧";
		 		ttmsm41.Update("REC_REVISOR,REC_REVISE_TIME,CURRENT_STATUS,TM_INSTALLATION_LOCATION,MOLD_NO", "MD_COPPER_PLATE_NO");
			}

		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

