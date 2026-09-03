/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-11
功能: 钢包烘烤实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm03.h"
#include "ttmsm01.h"

/*<remark>=========================================================
/// <summary>
/// 钢包烘烤实绩新增
/// <para>
/// 1.接收传入的钢包烘烤实绩信息；
/// 2.新增钢包烘烤实绩信息；
/// </para>
/// <para>数据库表：TTMSM03(钢包烘烤实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面烘烤实绩新增调用；TTMSM03画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的钢包烘烤实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm03_ins)

void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm03_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
		int	rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm03_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM03")
		{
			Log::Trace("","tmsm03_ins","rows [{0}]" ,rows);
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM03*/
				ttmsm03.Reset();
				/*读取传入的参数*/
				ttmsm03.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm03_ins","ttmsm03.SM_UNIT_NO [{0}]"	,(const char*)ttmsm03.SM_UNIT_NO);
				Log::Trace("","tmsm03_ins","ttmsm03.LADLE_NO [{0}]"		,(const char*)ttmsm03.LADLE_NO);			
				Log::Trace("","tmsm03_ins","ttmsm03.DRYING_ST [{0}]"	,(const char*)ttmsm03.DRYING_ST);
				Log::Trace("","tmsm03_ins","ttmsm03.DRYING_ET [{0}]"	,(const char*)ttmsm03.DRYING_ET);
				Log::Trace("","tmsm03_ins","ttmsm03.BAKE_TYPE [{0}]"	,(const char*)ttmsm03.BAKE_TYPE);
				Log::Trace("","tmsm03_ins","ttmsm03.BAKE_POS [{0}]"		,(const char*)ttmsm03.BAKE_POS);

				//班次班组 shift_no shift_group
				f_epep_get_shift_group("SM",ttmsm03.DRYING_ST,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				if(ttmsm03.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}
				if(ttmsm03.LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的钢包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}
				if(ttmsm03.DRYING_ST.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}
				//if(ttmsm03.DRYING_ET.Trim() == "")
				//{
				//	sprintf	(s.msg , "传入的烘烤结束时间不能为空") ;
				//	throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				//}
				if(ttmsm03.BAKE_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}
				if(ttmsm03.BAKE_POS.Trim() == "")
				{
					sprintf	(s.msg , "传入的烘烤位置不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}

				/*查询钢包烘烤实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM03 "
						 "  WHERE SM_UNIT_NO  = @ttmsm03.SM_UNIT_NO "
						 "  AND DRYING_ST = @ttmsm03.DRYING_ST "
						 "  AND BAKE_TYPE = @ttmsm03.BAKE_TYPE "
						 "  AND LADLE_NO = @ttmsm03.LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm03.SM_UNIT_NO", ttmsm03.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm03.DRYING_ST"	, ttmsm03.DRYING_ST );
				cmd_inq.Parameters.Set( "ttmsm03.BAKE_TYPE"	, ttmsm03.BAKE_TYPE );
				cmd_inq.Parameters.Set( "ttmsm03.LADLE_NO"	, ttmsm03.LADLE_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm03_ins","count [{0}]",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"钢包烘烤实绩信息已存在");
					/*新增钢包烘烤实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
				}
				
				// 生成烘烤流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(BAKE_SEQ_NO),0) "
							" FROM TTMSM03 "
							" WHERE SM_UNIT_NO	= @ttmsm03.SM_UNIT_NO "
							" AND LADLE_NO 		= @ttmsm03.LADLE_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm03.SM_UNIT_NO"	, ttmsm03.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm03.LADLE_NO"	, ttmsm03.LADLE_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm03.BAKE_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm03_ins","ttmsm03.BAKE_SEQ_NO [{0}]",ttmsm03.BAKE_SEQ_NO);

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
					Log::Trace("","tmsm03_ins","ttmsm03.ELAPSED_TIME [{0}]",ttmsm03.ELAPSED_TIME);
				}
				/*新增钢包烘烤实绩表*/
				ttmsm03.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm03.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
				//s.datetime是前台终端操作系统的时间，而不是服务器时间
				ttmsm03.TrimOrBlank();
				ttmsm03.Insert();
			}
		}
		else if(v_div.Trim() == "TMSM01")
		{
			/*重置TTMSM03*/
			ttmsm01.Reset();
			/*读取传入的参数*/
			ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm03_ins","ttmsm01.LADLE_NO [{0}]"		,(const char*)ttmsm01.LADLE_NO);
			Log::Trace("","tmsm03_ins","ttmsm01.SM_UNIT_NO [{0}]"	,(const char*)ttmsm01.SM_UNIT_NO);
			Log::Trace("","tmsm03_ins","ttmsm01.DRYING_ST [{0}]"	,(const char*)ttmsm01.DRYING_ST);
			Log::Trace("","tmsm03_ins","ttmsm01.DRYING_ET [{0}]"	,(const char*)ttmsm01.DRYING_ET);
			Log::Trace("","tmsm03_ins","ttmsm01.BAKE_TYPE [{0}]"	,(const char*)ttmsm01.BAKE_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			}
			if(ttmsm01.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			}
			if(ttmsm01.DRYING_ST.Trim() == "")
			{
				sprintf	(s.msg , "传入的烘烤起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			}
			//if(ttmsm01.DRYING_ET.Trim() == "")
			//{
			//	sprintf	(s.msg , "传入的烘烤结束时间不能为空") ;
			//	throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			//}
			if(ttmsm01.BAKE_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的烘烤类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			}

			//班次班组 shift_no shift_group
			f_epep_get_shift_group("SM",ttmsm01.DRYING_ST,v_shift,v_group,conn );

			/*查询钢包维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM03 "
					 "  WHERE SM_UNIT_NO	= @TTMSM03.SM_UNIT_NO "
					 "  AND DRYING_ST		= @TTMSM03.DRYING_ST "
					 "  AND BAKE_TYPE		= @TTMSM03.BAKE_TYPE "
					 "  AND LADLE_NO		= @TTMSM03.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "TTMSM03.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "TTMSM03.DRYING_ST"		, ttmsm01.DRYING_ST );
			cmd_inq.Parameters.Set( "TTMSM03.BAKE_TYPE"		, ttmsm01.BAKE_TYPE );
			cmd_inq.Parameters.Set( "TTMSM03.LADLE_NO"		, ttmsm01.LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm03_ins","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"钢包烘烤实绩信息已存在");
				/*新增钢包烘烤实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm03_ins");
			}
			
			// 生成烘烤流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(BAKE_SEQ_NO),0) "
						" FROM TTMSM03 "
						" WHERE SM_UNIT_NO	= @TTMSM03.SM_UNIT_NO "
						" AND LADLE_NO 		= @TTMSM03.LADLE_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("TTMSM03.SM_UNIT_NO"	, ttmsm01.SM_UNIT_NO);
			cmd_inq.Parameters.Set("TTMSM03.LADLE_NO"	, ttmsm01.LADLE_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm01.BAKE_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm03_ins","ttmsm01.BAKE_SEQ_NO [{0}]",ttmsm01.BAKE_SEQ_NO);

			if(ttmsm01.DRYING_ST.Trim() != "" && ttmsm01.DRYING_ET.Trim() != "")
			{
				v_drying_st = CDateTime::Parse(ttmsm01.DRYING_ST);
				v_drying_et = CDateTime::Parse(ttmsm01.DRYING_ET);
				v_elapsed_time = v_drying_et - v_drying_st;
				ttmsm01.ELAPSED_TIME = v_elapsed_time.TotalHours();
				Log::Trace("","tmsm03_ins","ttmsm01.ELAPSED_TIME [{0}]",ttmsm01.ELAPSED_TIME);
			}

			/*新增钢包烘烤实绩表*/
			ttmsm03.CopyFrom(ttmsm01);
			ttmsm03.Print();
			Log::Trace("","tmsm03_ins","ttmsm03.CopyFrom(ttmsm01);");
			ttmsm03.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm03.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm03.REC_REVISOR = " ";
			ttmsm03.REC_REVISE_TIME = " ";

			//s.datetime是前台终端操作系统的时间，而不是服务器时间
			if(ttmsm03.OPERATER.Trim() == "")
			{
				ttmsm03.OPERATER = s.userid;
			}
			Log::Trace("","tmsm03_ins","ttmsm03.OPERATER [{0}]",ttmsm03.OPERATER);
			if(ttmsm03.GRP_NO.Trim() == "")
			{
				ttmsm03.GRP_NO = v_group;
			}
			Log::Trace("","tmsm03_ins","ttmsm03.GRP_NO [{0}]",ttmsm03.GRP_NO);
			if(ttmsm03.SHIFT_NO.Trim() == "")
			{
				ttmsm03.SHIFT_NO = v_shift;
			}
			Log::Trace("","tmsm03_ins","ttmsm03.SHIFT_NO [{0}]",ttmsm03.SHIFT_NO);
			ttmsm03.TrimOrBlank();
			ttmsm03.Print();
			ttmsm03.Insert();
			Log::Trace("","tmsm03_ins","ttmsm03.Insert();");

			sqlstr = "UPDATE TTMSM01";
			// 00-新包 01-可用 02-凉包 03-黑包 11-烘烤 21-使用 31-维修 32-粘钢 99-报废
			ttmsm01.LADLE_STATUS = "11"; 
			ttmsm01.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,LADLE_STATUS "
							" ,REPAIR_SEQ_NO "
							" ,BAKE_SEQ_NO "	//烘烤流水号
							" ,BAKE_TYPE "		//烘烤类型
							" ,BAKE_POS "		//烘烤位置
							" ,DRYING_ST "		//烘烤开始时刻
							" ,DRYING_ET "		//烘烤结束时刻
							" ,ELAPSED_TIME "	//持续时间
							" ,BAKE_START_TEMP "//烘烤结束温度
							" ,BAKE_END_TEMP "	//烘烤结束温度
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