--------------------------------------------------
--    Created By: Luciano Morpurgo
--   Description: Trooper provides cover fire while other units are searching for enemy
--------------------------
--

AIBehaviour.TrGroupCoverSearch = {
	Name = "TrGroupCoverSearch",
	Base = "TrGroupSearch",
	TASK = 1,
	alertness = 1,

	Constructor = function ( self, entity,data )
		
		--TheOtherSide
		entity.AI.currentBehaviour = self.Name
		--~TheOtherSide	

		if ( AI.GetAIParameter( entity.id, AIPARAM_PERCEPTIONSCALE_VISUAL ) > 0.0 ) then

			-- data.point = enemy position
			-- data.point2 = average group position
			local position = g_Vectors.temp;
			FastSumVectors(position,data.point,data.point2);
			ScaleVectorInPlace(position,0.5);
			AI.SetRefPointPosition(entity.id,position);
			if(entity.AI.RefPointMemory==nil) then 
				entity.AI.RefPointMemory = {};	
			end
			CopyVector(entity.AI.RefPointMemory,data.point);
			entity:SelectPipe(0,"tr_approach_refpoint");
		end
	end,
	
	REFPOINT_REACHED = function(self,entity,sender)
		if ( AI.GetAIParameter( entity.id, AIPARAM_PERCEPTIONSCALE_VISUAL ) == 0.0 ) then
			return;
		end
		
		entity:SelectPipe(0,"do_nothing");
		local firepos = entity.AI.RefPointMemory;
		firepos.x = firepos.x + random(-3,3);
		firepos.y = firepos.y + random(-3,3);
		AI.SetRefPointPosition(entity.id,firepos);
		entity:InsertSubpipe(0,"tr_search_cover_fire");
	end,
	
}