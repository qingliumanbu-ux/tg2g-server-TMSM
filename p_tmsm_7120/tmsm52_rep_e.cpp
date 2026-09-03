/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车维修开始
**************************************************/
//框架头文件
#include "stdafx.h" 


//外部函数声明
int EPGetNextSeq(const char* seq_name, char* tcseq);
void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection* conn); //班次班别函数

BM2F_ENTERACE(tmsm52_rep_e)

int f_tmsm52_rep_e(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString strShiftNo("");
	CString	datetime_s("");
	CString strShiftGroup("");

	/* 实体类定义 */
	CModel ttmsm52("TTMSM52");
	CModel ttmsm57("TTMSM57");

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
			ttmsm57.Reset();
			ttmsm57.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm52.Reset();
			ttmsm52.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (bcls_rec->Tables[0].Columns.Contains("CRANENO"))
			{
				Log::Info("", __FUNCTION__, "CRANENO=[{0}]", ttmsm57["CRANENO"].ToString());
			}
			if (bcls_rec->Tables[0].Columns.Contains("REPAIR_END_TIME"))
			{
				datetime_s = ttmsm57["REPAIR_END_TIME"];
				Log::Info("", __FUNCTION__, "REPAIR_END_TIME=[{0}]", ttmsm57["REPAIR_END_TIME"].ToString());
			}


			ttmsm57["REC_REVISOR"] = s.userid;		//记录创建责任者
			ttmsm57["REC_REVISE_TIME"] = datetime;		//记录创建时刻
			ttmsm57.TrimOrBlank();
			/* 查询该行车状态是否存在 */
			//ttmsm52["CRANENO"] = ttmsm52["CRANENO"];
			if (ttmsm52.QueryCount("CRANENO")>0)
			{
				ttmsm52.Query("CRANENO");
				Log::Info("", __FUNCTION__, "CRANE_STAT=[{0}]", ttmsm52["CRANE_STAT"].ToString());
				if (ttmsm52["CRANE_STAT"].ToString().Trim() != '2')
				{
					sprintf(s.msg, "行车[%s],不在维修状态,不能结束！", (const char*)ttmsm52["CRANENO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsm57["REPAIR_SEQ_NO"] = ttmsm52["REPAIR_SEQ_NO"];
				/* 计算班组班别 */
				f_epep_get_shift_group("SM", datetime_s, strShiftNo, strShiftGroup, conn);
				ttmsm57["SHIFT_NO"] = strShiftNo;
				ttmsm57["SHIFT_GROUP"] = strShiftGroup;
				ttmsm57.Print();
				ttmsm57.Update("REC_REVISOR,REC_REVISE_TIME,REPAIR_END_TIME,SHIFT_NO,SHIFT_GROUP,RESP,REMARK","REPAIR_SEQ_NO");

				ttmsm52["REC_REVISOR"] = ttmsm57["REC_CREATOR"];
				ttmsm52["REC_REVISE_TIME"] = ttmsm57["REC_CREATE_TIME"];
				ttmsm52["CRANE_STAT"] = '0';
				ttmsm52.Print();
				ttmsm52.Update("REC_REVISOR,REC_REVISE_TIME,CRANE_STAT", "CRANENO");
				sprintf(s.msg, "处理正常结束");
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

