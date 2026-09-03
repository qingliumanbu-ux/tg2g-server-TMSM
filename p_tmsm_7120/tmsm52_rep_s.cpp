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
void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection* conn); //班次班别函数

BM2F_ENTERACE(tmsm52_rep_s)

int f_tmsm52_rep_s(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

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
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm57.Reset();
			ttmsm57.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm52.Reset();
			ttmsm52.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			if (bcls_rec->Tables[0].Columns.Contains("CRANENO"))
			{
				Log::Info("", __FUNCTION__, "CRANENO=[{0}]", ttmsm57["CRANENO"].ToString());
			}
			if (bcls_rec->Tables[0].Columns.Contains("REPAIR_START_TIME"))
			{
				datetime_s = ttmsm57["REPAIR_START_TIME"];
				Log::Info("", __FUNCTION__,"REPAIR_START_TIME=[{0}]", ttmsm57["REPAIR_START_TIME"].ToString());
			}

	 
			ttmsm57["REC_CREATOR"]			= s.userid;		//记录创建责任者
			ttmsm57["REC_CREATE_TIME"]		= datetime;		//记录创建时刻
			ttmsm57.TrimOrBlank();
			/* 查询该行车状态是否存在 */
			//ttmsm52["CRANENO"] = ttmsm52["CRANENO"];
			if (ttmsm52.Query("CRANENO"))
			{
				Log::Info("", __FUNCTION__, "CRANE_STAT=[{0}]", ttmsm52["CRANE_STAT"].ToString());
				if (ttmsm52["CRANE_STAT"].ToString().Trim() != '0')
				{
					sprintf(s.msg, "行车[%s],不在空闲状态,不能维修！", (const char*)ttmsm52["CRANENO"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			ttmsm57["REPAIR_SEQ_NO"] = EPGetNextSeq("TTMSM57_SEQ_NO", conn);
			/* 计算班组班别 */
			// f_epep_get_shift_group("SM", ttmsm57["REPAIR_START_TIME"].ToString(), ttmsm57["REPAIR_SHIFT_NO"].ToString(), ttmsm57["REPAIR_GRP_NO"].ToString(),conn);
			 f_epep_get_shift_group("SM",datetime_s,strShiftNo,strShiftGroup, conn);
			 ttmsm57["REPAIR_SHIFT_NO"]= strShiftNo;
			 ttmsm57["REPAIR_GRP_NO"]=strShiftGroup;
			 ttmsm57.Print();
			ttmsm57.Insert();


			//REC_REVISOR = :ttmsm57.rec_creator,/*记录修改责任者*/
				//REC_REVISE_TIME = : ttmsm57.rec_create_time,		/*记录修改时刻*/
				//REPAIR_SEQ_NO = : ttmsm57.repair_seq_no,		/*维修序号*/
				//CRANE_STAT = '2'		/*行车状态 2--维修*/
			ttmsm52["REC_REVISOR"] = ttmsm57["REC_CREATOR"];
			ttmsm52["REC_REVISE_TIME"] = ttmsm57["REC_CREATE_TIME"];
			ttmsm52["REPAIR_SEQ_NO"] = ttmsm57["REPAIR_SEQ_NO"];
			ttmsm52["CRANE_STAT"] = '2';
			ttmsm52.Print();
			ttmsm52.Update("REC_REVISOR,REC_REVISE_TIME,REPAIR_SEQ_NO,CRANE_STAT", "CRANENO");
			sprintf(s.msg, "处理正常结束");
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

