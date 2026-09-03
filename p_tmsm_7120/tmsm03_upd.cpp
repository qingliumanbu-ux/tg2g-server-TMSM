/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-11
功能: 钢包烘烤实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm03.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包烘烤实绩修改
/// <para>
/// 1.接收传入的钢包烘烤实绩信息；
/// 2.修改钢包烘烤实绩信息；
/// </para>
/// <para>数据库表：TTMSM03(钢包烘烤实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面烘烤实绩修改调用；TTMSM03画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的钢包烘烤实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm03_upd)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm03_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CDateTime v_drying_st;
	CDateTime v_drying_et;
	CTimeSpan v_elapsed_time;
	
	/* 实体类定义 */
	CTTMSM03 ttmsm03(conn);
	CTTMSM01 ttmsm01(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm03_upd","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM03")
		{
			//循环处理
			for (int i = 0; i < rows ; i++ )
			{
				// 取得单行传入信息 
				ttmsm03.Reset();
				ttmsm03.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				Log::Trace("","tmsm03_upd","ttmsm03.BAKE_SEQ_NO [{0}]"	,ttmsm03.BAKE_SEQ_NO);
				Log::Trace("","tmsm03_upd","ttmsm03.LADLE_NO [{0}]"		,(const char*)ttmsm03.LADLE_NO);
				Log::Trace("","tmsm03_upd","ttmsm03.SM_UNIT_NO [{0}]"	,(const char*)ttmsm03.SM_UNIT_NO);
				Log::Trace("","tmsm03_upd","ttmsm03.DRYING_ST [{0}]"	,(const char*)ttmsm03.DRYING_ST);
				Log::Trace("","tmsm03_upd","ttmsm03.DRYING_ET [{0}]"	,(const char*)ttmsm03.DRYING_ET);
				Log::Trace("","tmsm03_upd","ttmsm03.BAKE_TYPE [{0}]"	,(const char*)ttmsm03.BAKE_TYPE);
				Log::Trace("","tmsm03_upd","ttmsm03.BAKE_POS [{0}]"		,(const char*)ttmsm03.BAKE_POS);

				//出厂班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm03.DRYING_ST,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				if(ttmsm03.BAKE_SEQ_NO <= 0)
				{
					sprintf	(s.msg , "传入的烘烤流水号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.DRYING_ST.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.DRYING_ET.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.BAKE_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				if(ttmsm03.BAKE_POS.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤位置不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				
				/*查询钢包烘烤实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM03 "
						 "  WHERE BAKE_SEQ_NO	= @ttmsm03.BAKE_SEQ_NO "
						 "  AND SM_UNIT_NO		= @ttmsm03.SM_UNIT_NO "
						 "  AND LADLE_NO		= @ttmsm03.LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm03.BAKE_SEQ_NO"	, ttmsm03.BAKE_SEQ_NO );
				cmd_inq.Parameters.Set( "ttmsm03.LADLE_NO"		, ttmsm03.LADLE_NO );
				cmd_inq.Parameters.Set( "ttmsm03.SM_UNIT_NO"	, ttmsm03.SM_UNIT_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				Log::Trace("","tmsm03_upd","count {0}",count );
				cmd_inq.Close();
				if	(count == 0)
				{
					sprintf	(s.msg ,"钢包烘烤实绩信息不存在");
					/*新增钢包烘烤实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
				}
				
				//设置修改者信息
				ttmsm03.REC_REVISOR = s.userid;
				ttmsm03.REC_REVISE_TIME = datetimeNow;

				if(ttmsm03.OPERATER.Trim() == "")
				{
					ttmsm03.OPERATER = s.userid;
				}
				if(ttmsm03.GRP_NO.Trim() == "")
				{
					ttmsm03.GRP_NO = v_group;
				}
				if(ttmsm03.SHIFT_NO.Trim() == "")
				{
					ttmsm03.SHIFT_NO = v_shift;
				}

				if(ttmsm03.DRYING_ST.Trim() != "" && ttmsm03.DRYING_ET.Trim() != "")
				{
					v_drying_st = CDateTime::Parse(ttmsm03.DRYING_ST);
					v_drying_et = CDateTime::Parse(ttmsm03.DRYING_ET);
					v_elapsed_time = v_drying_et - v_drying_st;
					ttmsm03.ELAPSED_TIME = v_elapsed_time.TotalHours();
					Log::Trace("","tmsm03_upd","ttmsm03.ELAPSED_TIME [{0}]",ttmsm03.ELAPSED_TIME);
				}

				//根据传入信息更新钢包烘烤实绩表
				sqlstr = "UPDATE TTMSM03"; //用于捕获数据库操作异常情况
				ttmsm03.Update(	" REC_REVISOR, "
								" REC_REVISE_TIME, "
								" DRYING_ST, "		//烘烤开始时刻
								" DRYING_ET, "		//烘烤结束时刻
								" ELAPSED_TIME, "	//持续时间
								" BAKE_TYPE, "		//烘烤类型
								" BAKE_POS, "		//烘烤位置	
								" BAKE_START_TEMP, "  //烘烤结束温度
								" BAKE_END_TEMP, "	//烘烤结束温度
								" AFFIRM_FLAG, "	//确认标记
								" OPERATER, "		//操作者
								" GRP_NO, "			//班别号
								" SHIFT_NO, "		//班次号
								" REMARK, "		    //备注	
								" DRYING_REMARK, "  //烘烤备注
								" BAKE_SEQ_NO,SM_UNIT_NO,LADLE_NO ");
			}
		}
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM05*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm03_upd","ttmsm01.LADLE_NO [{0}]"		,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm03_upd","ttmsm01.SM_UNIT_NO [{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm03_upd","ttmsm01.DRYING_ST [{0}]"	,(const char*)ttmsm01.DRYING_ST);
			Log::Trace("","tmsm03_upd","ttmsm01.DRYING_ET [{0}]"	,(const char*)ttmsm01.DRYING_ET);
			Log::Trace("","tmsm03_upd","ttmsm01.BAKE_TYPE [{0}]"	,(const char*)ttmsm01.BAKE_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}
			if(ttmsm01.DRYING_ST.Trim() == "")
			{
				sprintf	(s.msg , "传入的烘烤起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}
			if(ttmsm01.DRYING_ET.Trim() == "")
			{
				sprintf	(s.msg , "传入的烘烤结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}
			if(ttmsm01.BAKE_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的烘烤类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}

			//班次班组 shift_no shift_group
			f_epep_get_shift_group("SM",ttmsm01.DRYING_ST,v_shift,v_group,conn );

			/*查询钢包烘烤实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM03 "
					 "  WHERE SM_UNIT_NO	= @ttmsm03.SM_UNIT_NO "					 
					 "  AND BAKE_TYPE		= @ttmsm03.BAKE_TYPE "
					 "  AND BAKE_SEQ_NO		= @ttmsm03.BAKE_SEQ_NO "
					 "  AND LADLE_NO		= @ttmsm03.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm03.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm03.BAKE_SEQ_NO"	, ttmsm01.BAKE_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm03.BAKE_TYPE"		, ttmsm01.BAKE_TYPE );
			cmd_inq.Parameters.Set( "ttmsm03.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm03_upd","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count == 0)
			{
				sprintf	(s.msg ,"钢包烘烤实绩信息不存在");
				/*新增钢包烘烤实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm03_upd");
			}

			if(ttmsm01.DRYING_ST.Trim() != "" && ttmsm01.DRYING_ET.Trim() != "")
			{
				v_drying_st = CDateTime::Parse(ttmsm01.DRYING_ST);
				v_drying_et = CDateTime::Parse(ttmsm01.DRYING_ET);
				v_elapsed_time = v_drying_et - v_drying_st;
				ttmsm01.ELAPSED_TIME = v_elapsed_time.TotalHours();
				Log::Trace("","tmsm03_upd","ttmsm01.ELAPSED_TIME [{0}]",ttmsm01.ELAPSED_TIME);
			}
			/*修改钢包烘烤实绩表*/
			ttmsm03.CopyFrom(ttmsm01);
			//ttmsm03.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			//ttmsm03.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm03.REC_REVISOR = s.userid;
			ttmsm03.REC_REVISE_TIME = datetimeNow;
			//班次班组 shift_no shift_group

			//s.datetime是前台终端操作系统的时间，而不是服务器时间
			if(ttmsm03.OPERATER.Trim() == "")
			{
				ttmsm03.OPERATER = s.userid;
			}
			if(ttmsm03.GRP_NO.Trim() == "")
			{
				ttmsm03.GRP_NO = v_group;
			}
			if(ttmsm03.SHIFT_NO.Trim() == "")
			{
				ttmsm03.SHIFT_NO = v_shift;
			}

			ttmsm03.TrimOrBlank();
			sqlstr = "UPDATE TTMSM03"; //用于捕获数据库操作异常情况
			ttmsm03.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,DRYING_ST "		//烘烤开始时刻
							" ,DRYING_ET "		//烘烤结束时刻
							" ,ELAPSED_TIME "	//持续时间
							" ,BAKE_TYPE "		//烘烤类型
							" ,BAKE_POS "		//烘烤位置
							" ,BAKE_START_TEMP "  //烘烤结束温度
							" ,BAKE_END_TEMP "  //烘烤结束温度
							" ,AFFIRM_FLAG "	//确认标记
							" ,OPERATER "		//操作者
							" ,GRP_NO "			//班别号
							" ,SHIFT_NO "		//班次号
							" ,DRYING_REMARK "  //烘烤备注
							" ,REMARK ",		//烘烤备注
							" BAKE_SEQ_NO,SM_UNIT_NO,LADLE_NO ");

			sqlstr = "UPDATE TTMSM01";
			ttmsm01.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,REPAIR_SEQ_NO "
							" ,BAKE_SEQ_NO "	//烘烤流水号
							" ,BAKE_TYPE "		//烘烤类型
							" ,BAKE_POS "		//烘烤位置
							" ,DRYING_ST "		//烘烤开始时刻
							" ,DRYING_ET "		//烘烤结束时刻
							" ,ELAPSED_TIME "	//持续时间
							" ,BAKE_START_TEMP "//烘烤结束温度
							" ,BAKE_END_TEMP "  //烘烤结束温度
							" ,DRYING_REMARK ",	//烘烤备注
							" SM_UNIT_NO,LADLE_NO ");
		}

		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	

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