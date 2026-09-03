/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 铁包使用实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm19.h"
#include "ttmsm11.h"

/*<remark>=========================================================
/// <summary>
/// 铁包使用实绩新增
/// <para>
/// 1.接收传入的铁包使用实绩信息；
/// 2.新增铁包使用实绩信息；
/// </para>
/// <para>数据库表：TTMSM19(铁包使用实绩表)      </para>
/// <para>主调用函数：前台TTMSM11画面使用实绩新增调用；TTMSM13画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的铁水罐使用实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm19_ins)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm19_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM19 ttmsm19(conn);
	CTTMSM11 ttmsm11(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		//记录总条数
		int	rows = bcls_rec->Tables[0].Rows.get_Count(); 
		Log::Trace("","tmsm19_ins","rows [{0}]" ,rows);
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm19_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM13")
		{
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM19*/
				ttmsm19.Reset();
				/*读取传入的参数*/
				ttmsm19.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm19_ins","ttmsm19.IRON_LADLE_NO [{0}]",(const char*)ttmsm19.IRON_LADLE_NO);
				Log::Trace("","tmsm19_ins","ttmsm19.SM_UNIT_NO [{0}]"	,(const char*)ttmsm19.SM_UNIT_NO);
				Log::Trace("","tmsm19_ins","ttmsm19.USAGE_ST [{0}]"		,(const char*)ttmsm19.USAGE_ST);
				Log::Trace("","tmsm19_ins","ttmsm19.USAGE_ET [{0}]"		,(const char*)ttmsm19.USAGE_ET);

				//班次班组 shift_no shift_group
				//f_epep_get_shift_group("DEFAULT",ttmsm19.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错			
				if(ttmsm19.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
				}
				if(ttmsm19.IRON_LADLE_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的铁包号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
				}
				if(ttmsm19.USAGE_ST.Trim() == "")
				{
					sprintf	(s.msg , "传入的使用起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
				}
				//if(ttmsm19.USAGE_ET.Trim() == "")
				//{
				//	sprintf	(s.msg , "传入的使用结束时间不能为空") ;
				//	throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
				//}

				//if(ttmsm19.PROD_DATE.Trim() != "")
				//{
				//	ttmsm19.PROD_DATE = ttmsm19.PROD_DATE.Substring(0,8);
				//}

				/*查询铁包使用实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM19 "
						 "  WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
						 "  AND USAGE_ST		= @ttmsm19.USAGE_ST "
						 "  AND IRON_LADLE_NO	= @ttmsm19.IRON_LADLE_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm19.SM_UNIT_NO"	, ttmsm19.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm19.USAGE_ST"		, ttmsm19.USAGE_ST );
				cmd_inq.Parameters.Set( "ttmsm19.IRON_LADLE_NO"	, ttmsm19.IRON_LADLE_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm19_ins","count {0}",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"铁包使用实绩信息已存在");
					/*新增铁包使用实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
				}
				
				// 生成使用流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
							" FROM TTMSM19 "
							" WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
							" AND IRON_LADLE_NO = @ttmsm19.IRON_LADLE_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm19.SM_UNIT_NO"		, ttmsm19.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm19.IRON_LADLE_NO"	, ttmsm19.IRON_LADLE_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm19.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm19_ins","ttmsm19.USE_SEQ_NO [{0}]",ttmsm19.USE_SEQ_NO);

				/*新增铁包使用实绩表*/
				ttmsm19.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm19.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 

				ttmsm19.TrimOrBlank();
				ttmsm19.Insert();
			}
		}
		else if(v_div.Trim() == "TMSM11")
		{
			/*重置TTMSM11*/
			ttmsm11.Reset();
			/*读取传入的参数*/
			ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm19_ins","ttmsm11.IRON_LADLE_NO [{0}]"		,(const char*)ttmsm11.IRON_LADLE_NO);
			Log::Trace("","tmsm19_ins","ttmsm11.SM_UNIT_NO [{0}]"	,(const char*)ttmsm11.SM_UNIT_NO);
			Log::Trace("","tmsm19_ins","ttmsm11.USAGE_ST [{0}]"		,(const char*)ttmsm11.USAGE_ST);
			Log::Trace("","tmsm19_ins","ttmsm11.USAGE_ET [{0}]"		,(const char*)ttmsm11.USAGE_ET);

			// 判传入的参数是否有错			
			if(ttmsm11.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
			}
			if(ttmsm11.IRON_LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的铁水罐号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
			}
			if(ttmsm11.USAGE_ST.Trim() == "")
			{
				sprintf	(s.msg , "传入的使用起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
			}
			//if(ttmsm11.USAGE_ET.Trim() == "")
			//{
			//	sprintf	(s.msg , "传入的使用结束时间不能为空") ;
			//	throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
			//}

			/*查询铁水罐使用实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM19 "
					 "  WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
					 "  AND USAGE_ST		= @ttmsm19.USAGE_ST "
					 "  AND IRON_LADLE_NO	= @ttmsm19.IRON_LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm19.SM_UNIT_NO"	, ttmsm11.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm19.USAGE_ST"		, ttmsm11.USAGE_ST );
			cmd_inq.Parameters.Set( "ttmsm19.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm19_ins","count {0}",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"铁水罐使用实绩信息已存在");
				/*新增铁水罐使用实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm19_ins");
			}
			
			// 生成使用流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(USE_SEQ_NO),0) "
						" FROM TTMSM19 "
						" WHERE SM_UNIT_NO	= @ttmsm19.SM_UNIT_NO "
						" AND IRON_LADLE_NO = @ttmsm19.IRON_LADLE_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm19.SM_UNIT_NO"		, ttmsm11.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm19.IRON_LADLE_NO"	, ttmsm11.IRON_LADLE_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm11.USE_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm19_ins","ttmsm11.USE_SEQ_NO [{0}]",ttmsm11.USE_SEQ_NO);

			/*新增铁水罐使用实绩表*/
			ttmsm19.CopyFrom(ttmsm11);
			ttmsm19.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm19.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm19.REC_REVISOR = " ";
			ttmsm19.REC_REVISE_TIME = " ";

			ttmsm19.TrimOrBlank();
			ttmsm19.Insert();

			sqlstr = "UPDATE TTMSM11";
			ttmsm11.MIT_STATUS = "11"; // 01-可用 11-使用 21-维修 99-报废
			ttmsm11.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,MIT_STATUS "
							" ,IRON_LADLE_POS " //铁水罐位置
							" ,WORK_MAKER "		//工作层(耐材厂家)
							" ,PROD_DATE "		//生产日期
							" ,USE_SEQ_NO "
							" ,MOLTIRON_WT "	//铁水重量
							" ,USAGE_ST "		//使用开始时间
							" ,USAGE_ET "		//使用结束时间
							" ,TPD_NO "			//倒罐处理号
							" ,IS_STANDARD_JAR "//是否标准罐
							" ,IRON_LTREAT_NO " //铁水预处理号
							" ,IS_IRON_LTREAT " //是否脱硫
							" ,LADLE_LIFE "		//包龄							
							" ,LADLE_ERA "		//包役
							" ,USE_REMARK "		//使用备注
							" ,IRON_LADLE_LIFE ",//铁水包包龄 //LADLE_LIFE
							" SM_UNIT_NO,IRON_LADLE_NO ");
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